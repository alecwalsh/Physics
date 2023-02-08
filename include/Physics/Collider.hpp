#pragma once

#include <glm/vec3.hpp>

#include <stdexcept>
#include <utility>
#include <type_traits>
#include <string>

#include <concepts>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {
    struct CollisionResult {
        bool collides = false;

        //Collider* collider1;
        //Collider* collider2;

        glm::vec3 normal = {};
        float penetration;

        constexpr CollisionResult(bool b) noexcept : collides{b} {}

        constexpr operator bool() const noexcept {
            return collides;
        }
    };

    // Forward declare classes
    class Collider;
    class SimplePlaneCollider;
    class SimpleCubeCollider;
    class SphereCollider;

    // Forward declare function templates
    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    CollisionResult Collides(const T& t, const U& u);

    // Returns true if collision checking between the types is implemented
    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    bool SupportsCollision() noexcept;

    class NotImplementedException : public std::runtime_error {
        static std::string CreateExceptionText(const Collider& collider1, const Collider& collider2);
    public:
        NotImplementedException(const Collider& collider1, const Collider& collider2) : runtime_error{CreateExceptionText(collider1, collider2)} {}
    };

    class CollisionDispatcher {
    protected:
        ~CollisionDispatcher() = default;
    public:
        CollisionDispatcher(const char* colliderTypeName) : colliderTypeName{colliderTypeName} {}

        virtual CollisionResult DispatchCollides(const SimplePlaneCollider&) const = 0;
        virtual CollisionResult DispatchCollides(const SimpleCubeCollider&) const = 0;
        virtual CollisionResult DispatchCollides(const SphereCollider&) const = 0;

        virtual bool DispatchCanCollide(const SimplePlaneCollider&) const noexcept = 0;
        virtual bool DispatchCanCollide(const SimpleCubeCollider&) const noexcept = 0;
        virtual bool DispatchCanCollide(const SphereCollider&) const noexcept = 0;

        const char* colliderTypeName;
    };

    template<typename ColliderType>
    class DispatcherCreator : public CollisionDispatcher {
        ColliderType* thisCollider;
    public:
        DispatcherCreator(ColliderType* thisCollider) : CollisionDispatcher{ColliderType::colliderTypeName}, thisCollider{thisCollider} {}

        CollisionResult DispatchCollides(const SimplePlaneCollider& other) const final {
            return Collides(*thisCollider, other);
        }
        CollisionResult DispatchCollides(const SimpleCubeCollider& other) const final {
            return Collides(*thisCollider, other);
        }
        CollisionResult DispatchCollides(const SphereCollider& other) const final {
            return Collides(*thisCollider, other);
        }

        bool DispatchCanCollide(const SimplePlaneCollider& other) const noexcept final {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            return SupportsCollision<ColliderType, OtherType>();
        }
        bool DispatchCanCollide(const SimpleCubeCollider& other) const noexcept final {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            return SupportsCollision<ColliderType, OtherType>();
        }
        bool DispatchCanCollide(const SphereCollider& other) const noexcept final {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            return SupportsCollision<ColliderType, OtherType>();
        }
    };

    class Collider {
    protected:
        virtual const CollisionDispatcher& GetCollisionDispatcher() const noexcept = 0;
        virtual CollisionDispatcher& GetCollisionDispatcher() noexcept = 0;
    public:
        glm::vec3 position = {};
        glm::vec3 size = {1, 1, 1};
        glm::vec3 velocity = {};

        bool hasGravity = true;
        std::string name;
        float restitution = 1;
        float mass = 1;

        Collider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity);
        Collider(glm::vec3 position, float size, glm::vec3 velocity);

        std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity(glm::vec3 gravityVector, float deltaTime) const noexcept;

        // Returns true if collision checking between this type and other's type is implemented
        virtual bool SupportsCollisionWith(const Collider& other) const noexcept = 0;

        virtual CollisionResult CollidesWith(const Collider&) const = 0;

        const char* GetColliderTypeName() const {
            return GetCollisionDispatcher().colliderTypeName;
        }

        virtual ~Collider() = default;
    };

    template<typename T>
    class ColliderCreator : public Collider {
        class Dispatcher final : public DispatcherCreator<T> {
        public:
            using DispatcherCreator<T>::DispatcherCreator;
        };

        Dispatcher dispatcher{static_cast<T*>(this)};

        const CollisionDispatcher& GetCollisionDispatcher() const noexcept final {
            return dispatcher;
        }
        CollisionDispatcher& GetCollisionDispatcher() noexcept final {
            return dispatcher;
        }
    public:
        using Collider::Collider;

        bool SupportsCollisionWith(const Collider& other) const noexcept final {
            return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCanCollide(*static_cast<const T*>(this));
        }

        CollisionResult CollidesWith(const Collider& other) const final {
            return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCollides(*static_cast<const T*>(this));
        }
    };

    class SimplePlaneCollider final : public ColliderCreator<SimplePlaneCollider> {
    public:
        static constexpr const char* colliderTypeName = "SimplePlane";

        SimplePlaneCollider(float height) : ColliderCreator{{0, height, 0}, 1, {}} {}
    };

    class SimpleCubeCollider final : public ColliderCreator<SimpleCubeCollider> {
    public:
        static constexpr const char* colliderTypeName = "SimpleCube";

        using ColliderCreator::ColliderCreator;
    };

    class SphereCollider final : public ColliderCreator<SphereCollider> {
    public:
        static constexpr const char* colliderTypeName = "Sphere";

        using ColliderCreator::ColliderCreator;

        // Ellipsoids are not supported
        SphereCollider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity) = delete;
    };

    constexpr float earthGravity = 9.81f;
    constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};
}

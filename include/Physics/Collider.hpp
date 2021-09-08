#pragma once

#include <glm/vec3.hpp>

#include <stdexcept>
#include <utility>
#include <type_traits>

#include <concepts>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {
    struct CollisionResult {
        bool collides = false;

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
    void ApplyCollisionToFirst(std::derived_from<Collider> auto&, const std::derived_from<Collider> auto&);

    // Returns true if collision checking between the types is implemented
    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    bool SupportsCollision() noexcept;

    class NotImplementedException : public std::runtime_error {
        std::string CreateExceptionText(const Collider& collider1, const Collider& collider2) const;
    public:
        NotImplementedException(const Collider& collider1, const Collider& collider2) : runtime_error{CreateExceptionText(collider1, collider2)} {}
    };

    class CollisionDispatcher {
    protected:
        ~CollisionDispatcher() = default;
    public:
        CollisionDispatcher(const char* name) : name{name} {}

        virtual CollisionResult DispatchCollides(const SimplePlaneCollider&) const = 0;
        virtual CollisionResult DispatchCollides(const SimpleCubeCollider&) const = 0;
        virtual CollisionResult DispatchCollides(const SphereCollider&) const = 0;

        virtual void DispatchApply(SimplePlaneCollider&) const = 0;
        virtual void DispatchApply(SimpleCubeCollider&) const = 0;
        virtual void DispatchApply(SphereCollider&) const = 0;

        virtual bool DispatchCanCollide(const SimplePlaneCollider&) const noexcept = 0;
        virtual bool DispatchCanCollide(const SimpleCubeCollider&) const noexcept = 0;
        virtual bool DispatchCanCollide(const SphereCollider&) const noexcept = 0;

        const char* name;
    };

    template<typename ColliderType>
    class DispatcherCreator : public CollisionDispatcher {
        ColliderType* thisCollider;
    public:
        DispatcherCreator(ColliderType* thisCollider) : CollisionDispatcher{ColliderType::name}, thisCollider{thisCollider} {}

        CollisionResult DispatchCollides(const SimplePlaneCollider& other) const override final {
            return Collides(*thisCollider, other);
        }
        CollisionResult DispatchCollides(const SimpleCubeCollider& other) const override final {
            return Collides(*thisCollider, other);
        }
        CollisionResult DispatchCollides(const SphereCollider& other) const override final {
            return Collides(*thisCollider, other);
        }

        void DispatchApply(SimplePlaneCollider& other) const override final {
            ApplyCollisionToFirst(other, *thisCollider);
        }
        void DispatchApply(SimpleCubeCollider& other) const override final {
            ApplyCollisionToFirst(other, *thisCollider);
        }
        void DispatchApply(SphereCollider& other) const override final {
            ApplyCollisionToFirst(other, *thisCollider);
        }

        bool DispatchCanCollide(const SimplePlaneCollider& other) const noexcept override final {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            return SupportsCollision<ColliderType, OtherType>();
        }
        bool DispatchCanCollide(const SimpleCubeCollider& other) const noexcept override final {
            using OtherType = std::remove_cvref_t<decltype(other)>;
            return SupportsCollision<ColliderType, OtherType>();
        }
        bool DispatchCanCollide(const SphereCollider& other) const noexcept override final {
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

        Collider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity);
        Collider(glm::vec3 position, float size, glm::vec3 velocity);

        std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const noexcept;

        // Returns true if collision checking between this type and other's type is implemented
        virtual bool SupportsCollisionWith(const Collider& other) const noexcept = 0;

        virtual CollisionResult CollidesWith(const Collider&) const = 0;

        // Updates velocity and position and applies collision detection
        virtual void ApplyCollision(const Collider&) = 0;

        const char* GetName() const {
            return GetCollisionDispatcher().name;
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

        const CollisionDispatcher& GetCollisionDispatcher() const noexcept override final {
            return dispatcher;
        }
        CollisionDispatcher& GetCollisionDispatcher() noexcept override final {
            return dispatcher;
        }
    public:
        using Collider::Collider;

        bool SupportsCollisionWith(const Collider& other) const noexcept override final {
            return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCanCollide(*static_cast<const T*>(this));
        }

        CollisionResult CollidesWith(const Collider& other) const override final {
            return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCollides(*static_cast<const T*>(this));
        }

        void ApplyCollision(const Collider& other) override final {
            static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchApply(*static_cast<T*>(this));
        }
    };

    class SimplePlaneCollider final : public ColliderCreator<SimplePlaneCollider> {
    public:
        static constexpr const char* name = "SimplePlane";

        SimplePlaneCollider(float height) : ColliderCreator{{0, height, 0}, 1, {}} {}
    };

    class SimpleCubeCollider final : public ColliderCreator<SimpleCubeCollider> {
    public:
        static constexpr const char* name = "SimpleCube";

        using ColliderCreator::ColliderCreator;
    };

    class SphereCollider final : public ColliderCreator<SphereCollider> {
    public:
        static constexpr const char* name = "Sphere";

        using ColliderCreator::ColliderCreator;

        // Ellipsoids are not supported
        SphereCollider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity) = delete;
    };

    constexpr float earthGravity = 9.81f;
    constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

    inline TimeManagerShim* timeManager;
}

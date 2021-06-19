#pragma once

#include <glm/vec3.hpp>

#include <vector>
#include <stdexcept>
#include <utility>
#include <type_traits>

#if __cpp_concepts >= 201907L
#include <concepts>
#endif

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {

class Collider;
class SimplePlaneCollider;
class SimpleCubeCollider;
class SphereCollider;

class NotImplementedException : public std::runtime_error {
    std::string CreateExceptionText(const Collider& collider1, const Collider& collider2) const;
public:
    NotImplementedException(const Collider& collider1, const Collider& collider2) : runtime_error{CreateExceptionText(collider1, collider2)} {}
};

#if __cpp_concepts >= 201907L
bool Collides(const std::derived_from<Collider> auto&, const std::derived_from<Collider> auto&);
void ApplyCollisionToFirst(std::derived_from<Collider> auto&, const std::derived_from<Collider> auto&);

// Returns true if collision checking between the types is implemented
template<std::derived_from<Collider> T, std::derived_from<Collider> U>
bool SupportsCollision() noexcept;
#else
template<typename T, typename U>
bool Collides(const T&, const U&);

template<typename T, typename U>
void ApplyCollisionToFirst(T&, const U&);

// Returns true if collision checking between the types is implemented
template<typename T, typename U>
bool SupportsCollision() noexcept;
#endif

class CollisionDispatcher {
protected:
    ~CollisionDispatcher() = default;
public:
    CollisionDispatcher(const char* name) : name{name} {}

    virtual bool DispatchCollides(const SimplePlaneCollider&) const = 0;
    virtual bool DispatchCollides(const SimpleCubeCollider&) const = 0;
    virtual bool DispatchCollides(const SphereCollider&) const = 0;

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
    
    bool DispatchCollides(const SimplePlaneCollider& other) const override {
        return Collides(*thisCollider, other);
    }
    bool DispatchCollides(const SimpleCubeCollider& other) const override {
        return Collides(*thisCollider, other);
    }
    bool DispatchCollides(const SphereCollider& other) const override {
        return Collides(*thisCollider, other);
    }

    void DispatchApply(SimplePlaneCollider& other) const override {
        ApplyCollisionToFirst(other, *thisCollider);
    }
    void DispatchApply(SimpleCubeCollider& other) const override {
        ApplyCollisionToFirst(other, *thisCollider);
    }
    void DispatchApply(SphereCollider& other) const override {
        ApplyCollisionToFirst(other, *thisCollider);
    }

    bool DispatchCanCollide(const SimplePlaneCollider& other) const noexcept override {
        using OtherType = std::remove_cv_t<std::remove_reference_t<decltype(other)>>;
        return SupportsCollision<ColliderType, OtherType>();
    }
    bool DispatchCanCollide(const SimpleCubeCollider& other) const noexcept override {
        using OtherType = std::remove_cv_t<std::remove_reference_t<decltype(other)>>;
        return SupportsCollision<ColliderType, OtherType>();
    }
    bool DispatchCanCollide(const SphereCollider& other) const noexcept override {
        using OtherType = std::remove_cv_t<std::remove_reference_t<decltype(other)>>;
        return SupportsCollision<ColliderType, OtherType>();
    }
};

class Collider {
protected:
    virtual const CollisionDispatcher& GetCollisionDispatcher() const noexcept = 0;
    virtual CollisionDispatcher& GetCollisionDispatcher() noexcept = 0;
public:
    glm::vec3 position = {};
    glm::vec3 size = { 1, 1, 1 };
    glm::vec3 velocity = {};

    Collider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity);
    Collider(glm::vec3 position, float size, glm::vec3 velocity);

    std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const noexcept;

    // Returns true if collision checking between this type and other's type is implemented
    virtual bool SupportsCollisionWith(const Collider& other) const noexcept = 0;

    virtual bool CollidesWith(const Collider&) const = 0;

    // Updates velocity and position and applies collision detection
    virtual void ApplyCollision(const Collider&) = 0;

    const char* GetName() const {
        return GetCollisionDispatcher().name;
    }

    virtual ~Collider() = default;
};

template<typename T>
class ColliderCreator : public Collider {
    class Dispatcher : public DispatcherCreator<T> {
    public:
        using DispatcherCreator<T>::DispatcherCreator;
    };

    Dispatcher dispatcher{static_cast<T*>(this)};

    const CollisionDispatcher& GetCollisionDispatcher() const noexcept override {
        return dispatcher;
    }
    CollisionDispatcher& GetCollisionDispatcher() noexcept override {
        return dispatcher;
    }
public:
    using Collider::Collider;

    bool SupportsCollisionWith(const Collider& other) const noexcept override {
        return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCanCollide(*static_cast<const T*>(this));
    }

    bool CollidesWith(const Collider& other) const override {
        return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchCollides(*static_cast<const T*>(this));
    }

    void ApplyCollision(const Collider& other) override {
        static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().DispatchApply(*static_cast<T*>(this));
    }
};

class SimplePlaneCollider : public ColliderCreator<SimplePlaneCollider> {
public:
    static constexpr const char* name = "SimplePlane";

    SimplePlaneCollider(float height) : ColliderCreator{{0, height, 0}, 1, {}} {}
};

class SimpleCubeCollider : public ColliderCreator<SimpleCubeCollider> {
public:
    static constexpr const char* name = "SimpleCube";

    using ColliderCreator::ColliderCreator;
};

class SphereCollider : public ColliderCreator<SphereCollider> {
public:
    static constexpr const char* name = "Sphere";

    using ColliderCreator::ColliderCreator;

    // Ellipsoids are not supported
    SphereCollider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity) = delete;
};

void collideAll(const std::vector<Collider*>& colliders);

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;
};
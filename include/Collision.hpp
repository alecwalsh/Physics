#pragma once

#include <glm/vec3.hpp>

//#include <functional>
#include <vector>
#include <stdexcept>
#include <utility>

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
#else
template<typename T, typename U>
bool Collides(const T&, const U&);

template<typename T, typename U>
void ApplyCollisionToFirst(T&, const U&);
#endif


class CollisionDispatcher {
public:
    CollisionDispatcher(const char* name) : name{name} {}

    virtual bool DispatchCollides(const SimplePlaneCollider&) const = 0;
    virtual bool DispatchCollides(const SimpleCubeCollider&) const = 0;
    virtual bool DispatchCollides(const SphereCollider&) const = 0;

    virtual void DispatchApply(SimplePlaneCollider&) const = 0;
    virtual void DispatchApply(SimpleCubeCollider&) const = 0;
    virtual void DispatchApply(SphereCollider&) const = 0;

    const char* name;
};

template<typename Collider>
class DispatcherCreator : public CollisionDispatcher {
    Collider* thisCollider;
public:
    DispatcherCreator(Collider* thisCollider) : CollisionDispatcher{Collider::name}, thisCollider{thisCollider} {}
    
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
};

class Collider {
protected:
    virtual const CollisionDispatcher& GetCollisionDispatcher() const = 0;
    virtual CollisionDispatcher& GetCollisionDispatcher() = 0;
public:
    glm::vec3 position = {};
    float size = 1;
    glm::vec3 velocity = {};

    Collider(glm::vec3 position, float size, glm::vec3 velocity);

    constexpr std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const;

    virtual bool CollidesWith(const Collider&) const = 0;

    // Updates velocity and position and applies collision detection
    virtual void ApplyCollision(const Collider&) = 0;

    const char* GetName() const {
        return GetCollisionDispatcher().name;
    }
};

template<typename T>
class ColliderCreator : public Collider {
    class Dispatcher : public DispatcherCreator<T> {
    public:
        using DispatcherCreator<T>::DispatcherCreator;
    };

    Dispatcher dispatcher{static_cast<T*>(this)};

    const CollisionDispatcher& GetCollisionDispatcher() const override {
        return dispatcher;
    }
    CollisionDispatcher& GetCollisionDispatcher() override {
        return dispatcher;
    }
public:
    using Collider::Collider;

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

    using ColliderCreator::ColliderCreator;
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
};

void collideAll(const std::vector<Collider*>& colliders);

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;

};
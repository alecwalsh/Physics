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
#else
template<typename T, typename U>
bool Collides(const T&, const U&);
#endif


class CollisionDispatcher {
public:
    CollisionDispatcher(const char* name) : name{name} {}

    virtual bool Dispatch(const SimplePlaneCollider&) const = 0;
    virtual bool Dispatch(const SimpleCubeCollider&) const = 0;
    virtual bool Dispatch(const SphereCollider&) const = 0;

    const char* name;
};

template<typename Collider>
class DispatcherCreator : public CollisionDispatcher {
    const Collider* thisCollider;
public:
    DispatcherCreator(const Collider* thisCollider) : CollisionDispatcher{Collider::name}, thisCollider{thisCollider} {}
    
    bool Dispatch(const SimplePlaneCollider& other) const override {
        return Collides(*thisCollider, other);
    }
    bool Dispatch(const SimpleCubeCollider& other) const override {
        return Collides(*thisCollider, other);
    }
    bool Dispatch(const SphereCollider& other) const override {
        return Collides(*thisCollider, other);
    }
};

class Collider {
protected:
    virtual const CollisionDispatcher& GetCollisionDispatcher() const = 0;
public:
    glm::vec3 position = {};
    float size = 1;
    glm::vec3 velocity = {};

    Collider(glm::vec3 position, float size, glm::vec3 velocity);

    constexpr std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const;

    virtual bool CollidesWith(const Collider&) const = 0;

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
public:
    using Collider::Collider;

    bool CollidesWith(const Collider& other) const override {
        return static_cast<const ColliderCreator&>(other).GetCollisionDispatcher().Dispatch(*static_cast<const T*>(this));
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

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SimplePlaneCollider& planeCollider);
};

class SphereCollider : public ColliderCreator<SphereCollider> {
    glm::vec3 SmallestY(const SphereCollider& otherSphere, glm::vec3 vec) const;
public:
    static constexpr const char* name = "Sphere";

    using ColliderCreator::ColliderCreator;

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SphereCollider& otherSphere);
};

void collideAll(const std::vector<Collider*>& colliders);

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;

};
#pragma once

#include <glm/vec3.hpp>

//#include <functional>
#include <vector>
#include <utility>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {

class SimplePlaneCollider;
class SimpleCubeCollider;
class SphereCollider;

    namespace Collision {
        bool Collides(const SimpleCubeCollider&, const SimpleCubeCollider&);

        bool Collides(const SimpleCubeCollider&, const SphereCollider&);

        bool Collides(const SphereCollider&, const SimpleCubeCollider&);

        bool Collides(const SphereCollider&, const SphereCollider&);
    }

class CollisionDispatcher {
    const char* type;
public:
    CollisionDispatcher(const char* type) : type{type} {}

    virtual bool CollidesWith(const SimplePlaneCollider&) = 0;
    virtual bool CollidesWith(const SimpleCubeCollider&) = 0;
    virtual bool CollidesWith(const SphereCollider&) = 0;

    const char* GetType() const {
        return type;
    }
};

class Collider {
public:
    glm::vec3 position = {};
    float size = 1;
    glm::vec3 velocity = {};

    Collider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg);

    constexpr std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const;

    virtual bool CollidesWith(const Collider&) const = 0;
    virtual CollisionDispatcher* GetCollisionDispatcher() const = 0;
};

class SimplePlaneCollider : public Collider {
    class SimplePlaneDispatcher : public CollisionDispatcher {
        const SimplePlaneCollider* thisCollider;
    public:
        SimplePlaneDispatcher(const SimplePlaneCollider* thisCollider) : CollisionDispatcher{"SimplePlane"}, thisCollider{thisCollider} {}

        bool CollidesWith(const SimplePlaneCollider&) override;
        bool CollidesWith(const SimpleCubeCollider&) override;
        bool CollidesWith(const SphereCollider&) override;
    };
public:
    SimplePlaneCollider(float height) : Collider{{0, height, 0}, 1, {}, false} {}
    bool CollidesWith(const Collider& other) const override {
        return other.GetCollisionDispatcher()->CollidesWith(*this);
    }

    CollisionDispatcher* GetCollisionDispatcher() const override {
        return new SimplePlaneDispatcher{this};
    }
};

class SimpleCubeCollider : public Collider {
    class SimpleCubeDispatcher : public CollisionDispatcher {
        const SimpleCubeCollider* thisCollider;
    public:
        SimpleCubeDispatcher(const SimpleCubeCollider* thisCollider) : CollisionDispatcher{"SimpleCube"}, thisCollider{thisCollider} {}

        bool CollidesWith(const SimplePlaneCollider&) override;
        bool CollidesWith(const SimpleCubeCollider&) override;
        bool CollidesWith(const SphereCollider&) override;
    };
public:
    SimpleCubeCollider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg);

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SimplePlaneCollider& planeCollider);

    bool CollidesWith(const Collider& other) const override {
        return other.GetCollisionDispatcher()->CollidesWith(*this);
    }

    CollisionDispatcher* GetCollisionDispatcher() const override {
        return new SimpleCubeDispatcher{this};
    }
};

class SphereCollider : public Collider {
    class SphereDispatcher : public CollisionDispatcher {
        const SphereCollider* thisCollider;
    public:
        SphereDispatcher(const SphereCollider* thisCollider) : CollisionDispatcher{"Sphere"}, thisCollider{thisCollider} {}

        bool CollidesWith(const SimplePlaneCollider&) override;
        bool CollidesWith(const SimpleCubeCollider&) override;
        bool CollidesWith(const SphereCollider&) override;
    };
public:
    SphereCollider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg);

    glm::vec3 SmallestY(const SphereCollider& otherSphere, glm::vec3 vec) const;

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SphereCollider& otherSphere);

    bool CollidesWith(const Collider& other) const override {
        return other.GetCollisionDispatcher()->CollidesWith(*this);
    }

    CollisionDispatcher* GetCollisionDispatcher() const override {
        return new SphereDispatcher{this};
    }
};

void collideAll(const std::vector<Collider*>& colliders);

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;

};
#pragma once

#include <glm/vec3.hpp>

//#include <functional>
#include <utility>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {

class Collider {
public:
    glm::vec3 velocity = {};
    glm::vec3 position = {};
    float size = 1;

    constexpr std::pair<glm::vec3, glm::vec3> CalculatePositionAndVelocity() const;
};

struct SimplePlaneCollider {
    float height = 0;
};

class SimpleCubeCollider : public Collider {
public:
    constexpr bool CollidesWith(const SimplePlaneCollider& planeCollider) const;

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SimplePlaneCollider& planeCollider);
};

class SphereCollider : public Collider {
public:
    glm::vec3 SmallestY(const SphereCollider& otherSphere, glm::vec3 vec) const;
    bool CollidesWith(const SphereCollider& sphere2) const;
    constexpr bool CollidesWith(const SimplePlaneCollider& planeCollider) const;

    // Updates velocity and position and applies collision detection
    void ApplyCollision(const SphereCollider& otherSphere);
};

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;

};
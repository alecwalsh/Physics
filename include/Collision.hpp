#pragma once

#include <glm/vec3.hpp>

//#include <functional>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {

class Collider {
public:
    glm::vec3 velocity = {};

    constexpr glm::vec3 calculateDistance(float deltaTime);
};

struct SimplePlaneCollider {
    float height = 0;
};

class SimpleCubeCollider : public Collider {
public:
    glm::vec3 position = {};
    float size = 1;

    constexpr bool collidesWith(const SimplePlaneCollider& planeCollider) const;

    // Applies gravity and collision detection and return a translation to be applied to the object
    glm::vec3 getTranslation(const SimplePlaneCollider& planeCollider);
};

class SphereCollider : public Collider {
public:
    glm::vec3 position = {};
    float diameter = 1;

    glm::vec3 smallestY(const SphereCollider& otherSphere, glm::vec3 vec) const;
    bool CollidesWith(const SphereCollider& sphere2) const;
    constexpr bool collidesWith(const SimplePlaneCollider& planeCollider) const;

    // Applies gravity and collision detection and return a translation to be applied to the object
    glm::vec3 getTranslation(const SphereCollider& otherSphere);
};

constexpr float earthGravity = 9.81f;
constexpr glm::vec3 earthGravityVector = {0, -earthGravity, 0};

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;
};
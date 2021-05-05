#pragma once

#include <glm/vec3.hpp>

//#include <functional>

struct TimeManagerShim {
    double& elapsedTime;
    double& deltaTime;
};

namespace Physics {

struct SphereCollider {
    glm::vec3 position;
    float radius;

    glm::vec3 velocity;
};

struct SimpleCubeCollider {
    glm::vec3 position;
    float size;

    glm::vec3 velocity;
};

struct SimplePlaneCollider {
    float height = 0;
};

constexpr float earthGravity = 9.81f;

// Applies gravity and collision detection and return a translation to be applied to the object
glm::vec3 getTranslation(SimpleCubeCollider& cubeCollider, const SimplePlaneCollider& planeCollider);

glm::vec3 getTranslation(SphereCollider& thisSphere, const SphereCollider& otherSphere);

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;
};
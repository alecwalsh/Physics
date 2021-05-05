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
};

struct SimpleCubeCollider {
    glm::vec3 position;
    float size;
};

struct SimplePlaneCollider {
    float height = 0;
};

constexpr float earthGravity = 9.81f;

glm::vec3 getTranslation(glm::vec3& velocity, const SimpleCubeCollider& cubeCollider, float floorHeight = 0);

// Applies gravity and collision detection and return a translation to be applied to the object
glm::vec3 getTranslation(glm::vec3& velocity, const SimpleCubeCollider& cubeCollider, const SimplePlaneCollider& planeCollider);

glm::vec3 getTranslationSphere(glm::vec3& velocity, const SphereCollider& thisSphere, const SphereCollider& otherSphere);

inline TimeManagerShim* timeManager;

//inline std::function<void(std::function<void()>)> addToUI;
};
#pragma once

#include "Collider.hpp"

#include <vector>

namespace Physics {

class PhysicsWorld {
    std::vector<Collider*> physicsObjects;

    float lastUpdate = 0;
    float tickRate = 60;

    glm::vec3 gravityVector = {0, -Physics::earthGravity, 0};
public:
    TimeManagerShim* timeManager;

    void Tick();

    void TickUntil();

    void AddPhysicsObject(Collider* collider);
};

}

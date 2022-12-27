#pragma once

#include "Collider.hpp"
#include "CollisionTest.hpp"

#include <span>

namespace Physics {
    void ResolveCollisions(std::span<Collider*> colliders);
    void ApplyVelocity(std::span<Collider*> colliders, glm::vec3 gravityVector, float deltaTime);
}

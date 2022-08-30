#pragma once

#include "Collider.hpp"
#include "CollisionTest.hpp"

#include <span>

namespace Physics {
    void ApplyVelocity(std::span<Collider*> colliders);
}

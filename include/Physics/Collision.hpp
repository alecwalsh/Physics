#pragma once

#include "Collider.hpp"
#include "ApplyCollision.hpp"
#include "CollisionTest.hpp"

#include <vector>

namespace Physics {
    void collideAll(const std::vector<Collider*>& colliders);
}

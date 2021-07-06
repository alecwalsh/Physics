#pragma once

#include "Collision.hpp"

namespace Physics {
    // Making this a struct with operator() allows us to use std::is_invocable
    struct CollidesImpl {
        CollisionResult operator()(const SimplePlaneCollider&, const SimplePlaneCollider&);
        CollisionResult operator()(const SimplePlaneCollider&, const SimpleCubeCollider&);
        CollisionResult operator()(const SimplePlaneCollider&, const SphereCollider&);

        CollisionResult operator()(const SimpleCubeCollider&, const SimpleCubeCollider&);
        CollisionResult operator()(const SimpleCubeCollider&, const SphereCollider&);

        CollisionResult operator()(const SphereCollider&, const SphereCollider&);
    };
}
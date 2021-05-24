#pragma once

#include "Collision.hpp"

namespace Physics {
    // Making this a struct with operator() allows us to use std::is_invocable
    struct CollidesImpl {
        bool operator()(const SimplePlaneCollider&, const SimplePlaneCollider&);
        bool operator()(const SimplePlaneCollider&, const SimpleCubeCollider&);
        bool operator()(const SimplePlaneCollider&, const SphereCollider&);

        bool operator()(const SimpleCubeCollider&, const SimpleCubeCollider&);
        bool operator()(const SimpleCubeCollider&, const SphereCollider&);

        bool operator()(const SphereCollider&, const SphereCollider&);
    };
}
#pragma once

#include "Collision.hpp"

namespace Physics {

#define IMPL_COLLIDES_DEFAULT(Type1, Type2) \
    bool operator()(const Type1& collider1, const Type2& collider2);

    // Making this a struct with operator() allows us to use std::is_invocable
    struct CollidesImpl {
        IMPL_COLLIDES_DEFAULT(SimplePlaneCollider, SimplePlaneCollider);

        bool operator()(const SimplePlaneCollider& collider1, const SimpleCubeCollider& collider2);

        bool operator()(const SimplePlaneCollider& collider1, const SphereCollider& collider2);

        IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SimpleCubeCollider);
        IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SphereCollider);

        bool operator()(const SphereCollider& collider1, const SphereCollider& collider2);
    };
#undef IMPL_COLLIDES_DEFAULT

}
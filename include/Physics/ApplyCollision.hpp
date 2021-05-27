#pragma once

#include "Collision.hpp"

namespace Physics {

#define DECLARE(Type1, Type2) \
    void ApplyCollisionImpl(Type1& collider1, const Type2& collider2);

DECLARE(SimplePlaneCollider, SimplePlaneCollider);
DECLARE(SimplePlaneCollider, SimpleCubeCollider);
DECLARE(SimplePlaneCollider, SphereCollider);

DECLARE(SimpleCubeCollider, SimplePlaneCollider);
DECLARE(SimpleCubeCollider, SimpleCubeCollider);
DECLARE(SimpleCubeCollider, SphereCollider);

DECLARE(SphereCollider, SimplePlaneCollider);
DECLARE(SphereCollider, SimpleCubeCollider);
DECLARE(SphereCollider, SphereCollider);

#undef DECLARE
}
#pragma once

#include "Collision.hpp"

namespace Physics {

#define IMPL_APPLY_COLLISION_DEFAULT(Type1, Type2) \
    void ApplyCollisionImpl(Type1& collider1, const Type2& collider2);

IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SimplePlaneCollider);
IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SimpleCubeCollider);
IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SphereCollider);

IMPL_APPLY_COLLISION_DEFAULT(SimpleCubeCollider, SimplePlaneCollider);
IMPL_APPLY_COLLISION_DEFAULT(SimpleCubeCollider, SimpleCubeCollider);
IMPL_APPLY_COLLISION_DEFAULT(SimpleCubeCollider, SphereCollider);

IMPL_APPLY_COLLISION_DEFAULT(SphereCollider, SimplePlaneCollider);
IMPL_APPLY_COLLISION_DEFAULT(SphereCollider, SimpleCubeCollider);
IMPL_APPLY_COLLISION_DEFAULT(SphereCollider, SphereCollider);

#undef IMPL_APPLY_COLLISION_DEFAULT
}
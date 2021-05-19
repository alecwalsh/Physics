#include "CollisionTest.hpp"

#include <glm/geometric.hpp>

namespace Physics {

#define IMPL_COLLIDES_DEFAULT(Type1, Type2) \
    bool CollidesImpl::operator()(const Type1& collider1, const Type2& collider2) { \
        throw NotImplementedException{collider1, collider2}; \
    }


    IMPL_COLLIDES_DEFAULT(SimplePlaneCollider, SimplePlaneCollider);

    bool CollidesImpl::operator()(const SimplePlaneCollider& collider1, const SimpleCubeCollider& collider2) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size / 2) <= planeHeight && (height + collider2.size / 2) >= planeHeight;
    }

    bool CollidesImpl::operator()(const SimplePlaneCollider& collider1, const SphereCollider& collider2) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size / 2) <= planeHeight && (height + collider2.size / 2) >= planeHeight;
    }

    IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SimpleCubeCollider);
    IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SphereCollider);

    bool CollidesImpl::operator()(const SphereCollider& collider1, const SphereCollider& collider2) {
        return glm::length(collider1.position - collider2.position) <= (collider1.size + collider2.size) / 2;
    }

#undef IMPL_COLLIDES_DEFAULT

}
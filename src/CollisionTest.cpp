#include "CollisionTest.hpp"

#include <glm/geometric.hpp>

namespace Physics {
    constexpr bool RangesOverlap(float min1, float max1, float min2, float max2) {
        return min1 < max2&& max1 > min2;
    }

    constexpr bool CheckRanges(float f1, float size1, float f2, float size2) {
        return RangesOverlap(f1 - size1 / 2, f1 + size1 / 2, f2 - size2 / 2, f2 + size2 / 2);
    }

    constexpr bool CubesCollideSimple(const Physics::SimpleCubeCollider& cube1, const Physics::SimpleCubeCollider& cube2) {
        auto position1 = cube1.position;
        auto size1 = cube1.size;
        auto position2 = cube2.position;
        auto size2 = cube2.size;

        bool xOverlap = CheckRanges(position1.x, size1.x, position2.x, size2.x);
        bool yOverlap = CheckRanges(position1.y, size1.y, position2.y, size2.y);
        bool zOverlap = CheckRanges(position1.z, size1.z, position2.z, size2.z);

        return xOverlap && yOverlap && zOverlap;
    }

#define IMPLEMENT(Type1, Type2) \
    CollisionResult CollidesImpl::operator()(const Type1& collider1, const Type2& collider2)

#define NOTIMPLEMENTED(Type1, Type2) \
    IMPLEMENT(Type1, Type2) { \
        throw NotImplementedException{collider1, collider2}; \
    }



    NOTIMPLEMENTED(SimplePlaneCollider, SimplePlaneCollider);

    IMPLEMENT(SimplePlaneCollider, SimpleCubeCollider) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size.y / 2) < planeHeight && (height + collider2.size.y / 2) > planeHeight;
    }

    IMPLEMENT(SimplePlaneCollider, SphereCollider) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size.y / 2) < planeHeight && (height + collider2.size.y / 2) > planeHeight;
    }

    IMPLEMENT(SimpleCubeCollider, SimpleCubeCollider) {
        return CubesCollideSimple(collider1, collider2);
    }

    NOTIMPLEMENTED(SimpleCubeCollider, SphereCollider);

    IMPLEMENT(SphereCollider, SphereCollider) {
        auto v = collider1.position - collider2.position;

        float r = (collider1.size + collider2.size).x / 2;
        float vv = glm::dot(v, v);

        CollisionResult result{vv < r * r};

        if(result.collides) {
            result.penetration = r - std::sqrt(vv);

            result.normal = v;
        }

        return result;
    }
}
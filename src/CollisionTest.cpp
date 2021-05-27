#include "CollisionTest.hpp"

#include <glm/geometric.hpp>

namespace Physics {

    namespace {
        constexpr bool RangesOverlap(float min1, float max1, float min2, float max2) {
            return min1 <= max2 && max1 >= min2;
        }

        constexpr bool CheckRanges(float f1, float size1, float f2, float size2) {
            return RangesOverlap(f1 - size1 / 2, f1 + size1 / 2, f2 - size2 / 2, f2 + size2 / 2);
        }

        constexpr bool CubesCollideSimple(const Physics::SimpleCubeCollider& cube1, const Physics::SimpleCubeCollider& cube2) {
            const auto& position1 = cube1.position;
            float size1 = cube1.size;
            const auto& position2 = cube2.position;
            float size2 = cube2.size;

            bool xOverlap = CheckRanges(position1.x, size1, position2.x, size2);
            bool yOverlap = CheckRanges(position1.y, size1, position2.y, size2);
            bool zOverlap = CheckRanges(position1.z, size1, position2.z, size2);

            return xOverlap && yOverlap && zOverlap;
        }
    }

#define NOTIMPLEMENTED(Type1, Type2) \
    bool CollidesImpl::operator()(const Type1& collider1, const Type2& collider2) { \
        throw NotImplementedException{collider1, collider2}; \
    }


    NOTIMPLEMENTED(SimplePlaneCollider, SimplePlaneCollider);

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

    bool CollidesImpl::operator()(const SimpleCubeCollider& collider1, const SimpleCubeCollider& collider2) {
        return CubesCollideSimple(collider1, collider2);
    }
    NOTIMPLEMENTED(SimpleCubeCollider, SphereCollider);

    bool CollidesImpl::operator()(const SphereCollider& collider1, const SphereCollider& collider2) {
        return glm::length(collider1.position - collider2.position) <= (collider1.size + collider2.size) / 2;
    }

#undef NOTIMPLEMENTED

}
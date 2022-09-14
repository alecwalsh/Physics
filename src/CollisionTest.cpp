#include "CollisionTest.hpp"

#include <glm/geometric.hpp>

#include <cassert>
#include <algorithm>

namespace Physics {
    constexpr bool RangesOverlap(float min1, float max1, float min2, float max2) {
        return min1 < max2 && max1 > min2;
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

        float r = collider2.size.y / 2;
        float d = std::abs(planeHeight - height);

        CollisionResult result{d < r};

        if(result.collides) {
            // collider2 is approaching the plane from above
            if(glm::dot(collider2.velocity, glm::vec3{0, -1, 0}) > 0) {
                result.penetration = r - (collider2.position.y - collider1.position.y);
                result.normal = glm::vec3{0, -1, 0};
            }
            // collider2 is approaching the plane from below
            if(glm::dot(collider2.velocity, glm::vec3{0, 1, 0}) > 0) {
                result.penetration = r + (collider2.position.y - collider1.position.y);
                result.normal = glm::vec3{0, 1, 0};
            }
        }

        return result;
    }

    IMPLEMENT(SimplePlaneCollider, SphereCollider) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        float r = collider2.size.y / 2;
        float d = std::abs(planeHeight - height);

        CollisionResult result{d < r};

        if(result.collides) {
            // collider2 is approaching the plane from above
            if(glm::dot(collider2.velocity, glm::vec3{0, -1, 0}) > 0) {
                result.penetration = r - (collider2.position.y - collider1.position.y);
                result.normal = glm::vec3{0, -1, 0};
            }
            // collider2 is approaching the plane from below
            if(glm::dot(collider2.velocity, glm::vec3{0, 1, 0}) > 0) {
                result.penetration = r + (collider2.position.y - collider1.position.y);
                result.normal = glm::vec3{0, 1, 0};
            }
        }

        return result;
    }

    IMPLEMENT(SimpleCubeCollider, SimpleCubeCollider) {
        bool collides = CubesCollideSimple(collider1, collider2);

        // Exit early if the cubes don't collide
        if (!collides) return collides;

        auto v = collider2.position - collider1.position;

        glm::vec3 a_extents = collider1.size / glm::vec3{2};
        glm::vec3 b_extents = collider2.size / glm::vec3{2};

        glm::vec3 overlaps = a_extents + b_extents - glm::abs(v);

        CollisionResult result{overlaps.x > 0 && overlaps.y > 0 && overlaps.z > 0};

        // This should have the same result as CubesCollideSimple
        assert(result.collides);

        float min_overlap = std::min({overlaps.x, overlaps.y, overlaps.z});

        if (min_overlap == overlaps.x) {
            if (v.x > 0) {
                result.normal = {-1, 0, 0};
            } else {
                result.normal = {1, 0, 0};
            }

            result.penetration = std::abs(overlaps.x);
        }

        if (min_overlap == overlaps.y) {
            if (v.y > 0) {
                result.normal = {0, -1, 0};
            } else {
                result.normal = {0, 1, 0};
            }

            result.penetration = std::abs(overlaps.y);
        }

        if (min_overlap == overlaps.z) {
            if (v.z > 0) {
                result.normal = {0, 0, -1};
            } else {
                result.normal = {0, 0, 1};
            }

            result.penetration = std::abs(overlaps.z);
        }

        return result;
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
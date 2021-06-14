#include "ApplyCollision.hpp"

#include <cmath>

namespace Physics {

namespace {
    void ApplyCollision(SphereCollider& collider1, const SimplePlaneCollider& planeCollider) {
        float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

        if(collider1.CollidesWith(planeCollider)) {
            // Is currently colliding with the floor
            collider1.velocity.y = 0;
            return;
        }

        auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

        auto newCollider = collider1;
        newCollider.position = newPosition;
        newCollider.velocity = newVelocity;
        // Is not currently colliding with the floor
        if(newCollider.CollidesWith(planeCollider)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor

            newCollider.position.y = planeCollider.position.y + collider1.size / 2;
        }

        collider1 = newCollider;
    }

    void ApplyCollision(SimpleCubeCollider& collider1, const SimplePlaneCollider& planeCollider) {
        float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

        if(collider1.CollidesWith(planeCollider)) {
            // Is currently colliding with the floor
            collider1.velocity.y = 0;
            return;
        }

        auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

        auto newCollider = collider1;
        newCollider.position = newPosition;
        newCollider.velocity = newVelocity;
        // Is not currently colliding with the floor
        if(newCollider.CollidesWith(planeCollider)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor

            newCollider.position.y = planeCollider.position.y + collider1.size / 2;
        }

        collider1 = newCollider;
    }

    // Applying vec to this sphere results in an intersection
    // This function finds the value for vec.y that makes the spheres touch, but not intersect
    // TODO: Use x, y, and z instead of just y
    glm::vec3 SmallestY(const SphereCollider& thisSphere, const SphereCollider& otherSphere, glm::vec3 vec) noexcept {
        using std::pow, std::abs, std::sqrt;

        auto dist2 = pow((thisSphere.size + otherSphere.size) / 2, 2);

        auto vecWithoutY = vec;
        vecWithoutY.y = 0;

        const auto p1 = thisSphere.position + vecWithoutY;
        const auto p2 = otherSphere.position;

        const auto a = 1.0;
        const auto b = 2 * (p1.y - p2.y);
        const auto c = pow(p1.x - p2.x, 2) + pow(p1.z - p2.z, 2) + pow(p1.y - p2.y, 2) - dist2;

        auto det = b * b - 4 * a * c;

        auto r1 = (-b + sqrt(det)) / (2 * a);
        auto r2 = (-b - sqrt(det)) / (2 * a);

        auto r1abs = abs(r1);
        auto r2abs = abs(r2);

        auto yDiff = r1abs < r2abs ? r1 : r2;

        glm::vec3 diff = {0, yDiff, 0};

        return diff;
    }

    void ApplyCollision(SphereCollider& collider1, const SphereCollider& otherSphere) {
        float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

        if(collider1.CollidesWith(otherSphere)) {
            // Is currently colliding with the other sphere
            collider1.velocity.y = 0;
            return;
        }

        // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
        // Is not currently colliding with the other sphere
        auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

        auto newCollider = collider1;
        newCollider.position = newPosition;
        newCollider.velocity = newVelocity;

        if(newCollider.CollidesWith(otherSphere)) {
            // The new position will collide with the other sphere
            newCollider.position = collider1.position + SmallestY(collider1, otherSphere, newPosition - collider1.position);
        }

        collider1 = newCollider;
    }
}

#define IMPLEMENT(Type1, Type2) \
    void ApplyCollisionImpl(Type1& collider1, const Type2& collider2) { \
        ApplyCollision(collider1, collider2); \
    }

#define NOTIMPLEMENTED(Type1, Type2) \
    void ApplyCollisionImpl(Type1& collider1, const Type2& collider2) { \
        throw NotImplementedException{collider1, collider2}; \
    }

    NOTIMPLEMENTED(SimplePlaneCollider, SimplePlaneCollider);
    NOTIMPLEMENTED(SimplePlaneCollider, SimpleCubeCollider);
    NOTIMPLEMENTED(SimplePlaneCollider, SphereCollider);


    IMPLEMENT(SimpleCubeCollider, SimplePlaneCollider)
    NOTIMPLEMENTED(SimpleCubeCollider, SimpleCubeCollider);
    NOTIMPLEMENTED(SimpleCubeCollider, SphereCollider);

    IMPLEMENT(SphereCollider, SimplePlaneCollider)
    NOTIMPLEMENTED(SphereCollider, SimpleCubeCollider);
    IMPLEMENT(SphereCollider, SphereCollider)

#undef IMPLEMENT
#undef NOTIMPLEMENTED
}
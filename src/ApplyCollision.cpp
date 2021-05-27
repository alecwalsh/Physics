#include "ApplyCollision.hpp"

#include <cmath>

namespace Physics {

namespace {
    void ApplyCollision(SphereCollider& collider1, const SimplePlaneCollider& planeCollider) {
        float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

        if(collider1.CollidesWith(planeCollider)) {
            // Is currently colliding with the floor
            collider1.velocity.y = 0;
        }
        else {
            auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

            //addToUI([distance, velocity] {
            //    ImGui::Text("Physics info:");
            //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
            //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
            //    ImGui::NewLine();
            //});

            auto newCollider = collider1;
            newCollider.position = newPosition;
            newCollider.velocity = newVelocity;
            // Is not currently colliding with the floor
            if(newCollider.CollidesWith(planeCollider)) {
                // The new height will collide with the floor
                // Set distance so that the new height is exactly at the floor

                //distance.y = planeCollider.height - position.y + size / 2;
                newCollider.position.y = planeCollider.position.y + collider1.size / 2;
            }

            collider1 = newCollider;
        }
    }
    void ApplyCollision(SimpleCubeCollider& collider1, const SimplePlaneCollider& planeCollider) {
        float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

        if(collider1.CollidesWith(planeCollider)) {
            // Is currently colliding with the floor
            collider1.velocity.y = 0;
        }
        else {
            auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

            //addToUI([distance, velocity] {
            //    ImGui::Text("Physics info:");
            //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
            //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
            //    ImGui::NewLine();
            //});

            auto newCollider = collider1;
            newCollider.position = newPosition;
            newCollider.velocity = newVelocity;
            // Is not currently colliding with the floor
            if(newCollider.CollidesWith(planeCollider)) {
                // The new height will collide with the floor
                // Set distance so that the new height is exactly at the floor

                //distance.y = planeCollider.height - position.y + size / 2;
                newCollider.position.y = planeCollider.position.y + collider1.size / 2;
            }

            collider1 = newCollider;
        }
    }

    // Applying vec to this sphere results in an intersection
    // This function finds the value for vec.y that makes the spheres touch, but not intersect
    // TODO: Use x, y, and z instead of just y
    glm::vec3 SmallestY(const SphereCollider& thisSphere, const SphereCollider& otherSphere, glm::vec3 vec) {
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
        }
        else {
            // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
            // Is not currently colliding with the other sphere
            auto [newPosition, newVelocity] = collider1.CalculatePositionAndVelocity();

            //addToUI([distance, velocity] {
            //    ImGui::Text("Physics info:");
            //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
            //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
            //    ImGui::NewLine();
            //});

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
}

#define IMPL_APPLY_COLLISION_DEFAULT(Type1, Type2) \
    void ApplyCollisionImpl(Type1& collider1, const Type2& collider2) { \
        throw NotImplementedException{collider1, collider2}; \
    }

    IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SimplePlaneCollider);
    IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SimpleCubeCollider);
    IMPL_APPLY_COLLISION_DEFAULT(SimplePlaneCollider, SphereCollider);

    void ApplyCollisionImpl(SimpleCubeCollider& collider1, const SimplePlaneCollider& collider2) {
        ApplyCollision(collider1, collider2);
    }
    IMPL_APPLY_COLLISION_DEFAULT(SimpleCubeCollider, SimpleCubeCollider);
    IMPL_APPLY_COLLISION_DEFAULT(SimpleCubeCollider, SphereCollider);

    void ApplyCollisionImpl(SphereCollider& collider1, const SimplePlaneCollider& collider2) {
        ApplyCollision(collider1, collider2);
    }
    IMPL_APPLY_COLLISION_DEFAULT(SphereCollider, SimpleCubeCollider);
    void ApplyCollisionImpl(SphereCollider& collider1, const SphereCollider& collider2) {
        ApplyCollision(collider1, collider2);
    }

#undef IMPL_APPLY_COLLISION_DEFAULT



}
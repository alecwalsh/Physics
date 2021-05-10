#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

constexpr bool SimpleCubeCollider::CollidesWith(const SimplePlaneCollider& planeCollider) const {
    auto height = position.y;

    return (height - size / 2) <= planeCollider.height && (height + size/2) >= planeCollider.height;
}

constexpr std::pair<glm::vec3, glm::vec3> Collider::CalculatePositionAndVelocity() const {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);
    const auto& acceleration = earthGravityVector;

    auto initialVelocity = velocity;

    auto newVelocity = velocity + acceleration * deltaTime;

    auto distance = initialVelocity * deltaTime + (acceleration * (deltaTime * deltaTime / 2));

    return {position + distance, newVelocity};
}

void SimpleCubeCollider::ApplyCollision(const SimplePlaneCollider& planeCollider) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    if(CollidesWith(planeCollider)) {
        // Is currently colliding with the floor
        velocity.y = 0;
    }
    else {
        auto [newPosition, newVelocity] = CalculatePositionAndVelocity();

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = *this;
        newCollider.position = newPosition;
        newCollider.velocity = newVelocity;
        // Is not currently colliding with the floor
        if(newCollider.CollidesWith(planeCollider)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            
            //distance.y = planeCollider.height - position.y + size / 2;
            newCollider.position.y = planeCollider.height + size / 2;
        }

        *this = newCollider;
    }
}

bool SphereCollider::CollidesWith(const SphereCollider& sphere2) const {
    return glm::length(position - sphere2.position) <= (size + sphere2.size) / 2;
}

constexpr bool SphereCollider::CollidesWith(const SimplePlaneCollider& planeCollider) const {
    auto height = position.y;

    return (height - size / 2) <= planeCollider.height && (height + size / 2) >= planeCollider.height;
}

// Applying vec to thisSphere result in an intersection
// This function finds the value for vec.y that makes the spheres touch, but not intersect
// TODO: Use x, y, and z instead of just y
glm::vec3 SphereCollider::SmallestY(const SphereCollider& otherSphere, glm::vec3 vec) const {
    using std::pow, std::abs, std::sqrt;

    auto dist2 = pow((size + otherSphere.size) / 2, 2);

    auto vecWithoutY = vec;
    vecWithoutY.y = 0;

    const auto p1 = position + vecWithoutY;
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

void SphereCollider::ApplyCollision(const SphereCollider& otherSphere) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    if(CollidesWith(otherSphere)) {
        // Is currently colliding with the other sphere
        velocity.y = 0;
    }
    else {
        // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
        // Is not currently colliding with the other sphere
        auto [newPosition, newVelocity] = CalculatePositionAndVelocity();

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = *this;
        newCollider.position = newPosition;
        newCollider.velocity = newVelocity;

        if(newCollider.CollidesWith(otherSphere)) {
            // The new position will collide with the other sphere
            newCollider.position += SmallestY(otherSphere, newPosition - position);
        }

        *this = newCollider;
    }
}

} // namespace Physics
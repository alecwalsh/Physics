#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

constexpr bool SimpleCubeCollider::collidesWith(const SimplePlaneCollider& planeCollider) const {
    auto height = position.y;

    return (height - size / 2) <= planeCollider.height && (height + size/2) >= planeCollider.height;
}

constexpr glm::vec3 Collider::calculateDistance(float deltaTime) {
    const auto& acceleration = earthGravityVector;

    auto initialVelocity = velocity;

    velocity += acceleration * deltaTime;

    auto distance = initialVelocity * deltaTime + (acceleration * (deltaTime * deltaTime / 2));

    return distance;
}

glm::vec3 SimpleCubeCollider::getTranslation(const SimplePlaneCollider& planeCollider) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    glm::vec3 distance{};

    if (collidesWith(planeCollider)) {
        // Is currently colliding with the floor
        distance.y = 0;
        velocity.y = 0;
    } else {
        distance = calculateDistance(deltaTime);

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = *this;
        newCollider.position += distance;
        // Is not currently colliding with the floor
        if (newCollider.collidesWith(planeCollider)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            distance.y = planeCollider.height - position.y + size / 2;
        }
    }

    return distance;
}

bool SphereCollider::CollidesWith(const SphereCollider& sphere2) const {
    return glm::length(position - sphere2.position) <= (diameter + sphere2.diameter)/2;
}

constexpr bool SphereCollider::collidesWith(const SimplePlaneCollider& planeCollider) const {
    auto height = position.y;

    return (height - diameter / 2) <= planeCollider.height && (height + diameter / 2) >= planeCollider.height;
}

// Applying vec to thisSphere result in an intersection
// This function finds the value for vec.y that makes the spheres touch, but not intersect
// TODO: Use x, y, and z instead of just y
glm::vec3 SphereCollider::smallestY(const SphereCollider& otherSphere, glm::vec3 vec) const {
    using std::pow, std::abs, std::sqrt;
    
    auto dist2 = pow((diameter + otherSphere.diameter) / 2, 2);

    auto vecWithoutY = vec;
    vecWithoutY.y = 0;
    
    const auto p1 = position + vecWithoutY;
    const auto p2 = otherSphere.position;

    const auto a = 1.0;
    const auto b = 2 * (p1.y - p2.y);
    const auto c = pow(p1.x - p2.x, 2) + pow(p1.z - p2.z, 2) + pow(p1.y - p2.y, 2) - dist2;
    
    auto det = b*b - 4*a*c;

    auto r1 = (-b + sqrt(det)) / (2 * a);
    auto r2 = (-b - sqrt(det)) / (2 * a);

    auto r1abs = abs(r1);
    auto r2abs = abs(r2);

    auto yDiff = r1abs < r2abs ? r1 : r2;

    glm::vec3 diff = {0, yDiff, 0};

    return vecWithoutY + diff;
}

glm::vec3 SphereCollider::getTranslation(const SphereCollider& otherSphere) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    glm::vec3 distance = velocity * deltaTime;

    if (CollidesWith(otherSphere)) {
        // Is currently colliding with the other sphere
        distance.y = 0;
        velocity.y = 0;
    } else {
        // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
        // Is not currently colliding with the other sphere
        distance = calculateDistance(deltaTime);

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = *this;
        newCollider.position += distance;

        if (newCollider.CollidesWith(otherSphere)) {
            // The new position will collide with the other sphere
            distance = smallestY(otherSphere, distance);
        }
    }

    return distance;
}

} // namespace Physics
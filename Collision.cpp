#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

constexpr bool collidesWithFloor(SimpleCubeCollider collider, float floorHeight) {
    auto height = collider.position.y;
    auto size = collider.size;

    return (height - size / 2) <= floorHeight && (height + size/2) >= floorHeight;
}

constexpr glm::vec3 calculateDistance(glm::vec3& velocity, float deltaTime) {
    const auto acceleration = glm::vec3{0, -earthGravity, 0};

    velocity += acceleration * deltaTime;

    auto distance = velocity * deltaTime + (acceleration * deltaTime * deltaTime) / glm::vec3{2, 2, 2};

    return distance;
}

glm::vec3 getTranslation(glm::vec3& velocity, const SimpleCubeCollider& collider, float floorHeight) {
    glm::vec3 distance{};

    if (collidesWithFloor(collider, floorHeight)) {
        // Is currently colliding with the floor
        distance.y = 0;
        velocity.y = 0;
    } else {
        distance = calculateDistance(velocity, static_cast<float>(Physics::timeManager->deltaTime));

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = collider;
        newCollider.position += distance;
        // Is not currently colliding with the floor
        if (collidesWithFloor(newCollider, floorHeight)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            distance.y = floorHeight - collider.position.y + collider.size / 2;
        }
    }

    return distance;
}

bool SpheresCollide(const SphereCollider& sphere1, const SphereCollider& sphere2) {
    return glm::length(sphere1.position - sphere2.position) <= (sphere1.radius + sphere2.radius);
}

glm::vec3 calculateDistanceSphere(glm::vec3& velocity, float deltaTime) {
    const float acceleration = earthGravity;

    velocity.y -= acceleration * deltaTime;

    glm::vec3 translation = velocity * deltaTime;

    float yDistance = (acceleration * deltaTime * deltaTime) / 2;

    translation.y -= yDistance;

    return translation;
}

// Applying vec to thisSphere result in an intersection
// This function finds the value for vec.y that makes the spheres touch, but not intersect
glm::vec3 smallestY(const SphereCollider& thisSphere, const SphereCollider& otherSphere, glm::vec3 vec) {
    using std::pow, std::abs, std::sqrt;
    
    auto dist2 = pow(thisSphere.radius + otherSphere.radius, 2);

    auto vecWithoutY = vec;
    vecWithoutY.y = 0;
    
    const auto p1 = thisSphere.position + vecWithoutY;
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

glm::vec3 getTranslationSphere(glm::vec3& velocity, const SphereCollider& thisSphere, const SphereCollider& otherSphere) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    glm::vec3 distance = velocity * deltaTime;

    if (SpheresCollide(thisSphere, otherSphere)) {
        // Is currently colliding with the other sphere
        distance.y = 0;
        velocity.y = 0;
    } else {
        // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
        // Is not currently colliding with the other sphere
        distance = calculateDistanceSphere(velocity, deltaTime);

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = thisSphere;
        newCollider.position += distance;

        if (SpheresCollide(newCollider, otherSphere)) {
            // The new position will collide with the other sphere
            distance = smallestY(thisSphere, otherSphere, distance);
        }
    }

    return distance;
}

} // namespace Physics
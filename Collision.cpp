#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

constexpr bool collidesWithFloor(const SimpleCubeCollider& collider, const SimplePlaneCollider& planeCollider) {
    auto height = collider.position.y;
    auto size = collider.size;

    return (height - size / 2) <= planeCollider.height && (height + size/2) >= planeCollider.height;
}

constexpr bool collidesWithFloor(const SphereCollider& collider, const SimplePlaneCollider& planeCollider) {
    auto height = collider.position.y;
    auto size = collider.radius * 2;

    return (height - size / 2) <= planeCollider.height && (height + size / 2) >= planeCollider.height;
}

constexpr glm::vec3 calculateDistance(SimpleCubeCollider& cubeCollider, float deltaTime) {
    const auto acceleration = glm::vec3{0, -earthGravity, 0};

    cubeCollider.velocity += acceleration * deltaTime;

    auto distance = cubeCollider.velocity * deltaTime + (acceleration * deltaTime * deltaTime) / glm::vec3{2, 2, 2};

    return distance;
}

glm::vec3 getTranslation(SimpleCubeCollider& cubeCollider, const SimplePlaneCollider& planeCollider) {
    glm::vec3 distance{};

    if (collidesWithFloor(cubeCollider, planeCollider)) {
        // Is currently colliding with the floor
        distance.y = 0;
        cubeCollider.velocity.y = 0;
    } else {
        distance = calculateDistance(cubeCollider, static_cast<float>(Physics::timeManager->deltaTime));

        //addToUI([distance, velocity] {
        //    ImGui::Text("Physics info:");
        //    ImGui::Text("velocity: %f, %f, %f", velocity.x, velocity.y, velocity.z);
        //    ImGui::Text("distance: %f, %f, %f", distance.x, distance.y, distance.z);
        //    ImGui::NewLine();
        //});

        auto newCollider = cubeCollider;
        newCollider.position += distance;
        // Is not currently colliding with the floor
        if (collidesWithFloor(newCollider, planeCollider)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            distance.y = planeCollider.height - cubeCollider.position.y + cubeCollider.size / 2;
        }
    }

    return distance;
}

bool SpheresCollide(const SphereCollider& sphere1, const SphereCollider& sphere2) {
    return glm::length(sphere1.position - sphere2.position) <= (sphere1.radius + sphere2.radius);
}

glm::vec3 calculateDistance(SphereCollider& sphereCollider, float deltaTime) {
    const glm::vec3 acceleration = {0, earthGravity, 0};

    sphereCollider.velocity -= acceleration * deltaTime;

    glm::vec3 translation = sphereCollider.velocity * deltaTime;

    auto v1 = glm::vec3{2};
    assert(v1.x == v1.y && v1.y == v1.z);

    glm::vec3 distance = (acceleration * deltaTime * deltaTime) / glm::vec3{2};

    translation -= distance;

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

glm::vec3 getTranslation(SphereCollider& thisSphere, const SphereCollider& otherSphere) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    glm::vec3 distance = thisSphere.velocity * deltaTime;

    if (SpheresCollide(thisSphere, otherSphere)) {
        // Is currently colliding with the other sphere
        distance.y = 0;
        thisSphere.velocity.y = 0;
    } else {
        // TODO: Don't apply full velocity here because the sphere is only moving partway through distance
        // Is not currently colliding with the other sphere
        distance = calculateDistance(thisSphere, deltaTime);

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
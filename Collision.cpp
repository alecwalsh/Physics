#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

#include <sstream>
#include <stdexcept>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

    namespace Collision {
        class CollisionNotImplementedException : public std::runtime_error {
            auto CreateExceptionText(const Collider& collider1, const Collider& collider2) {
                auto type1 = collider1.GetCollisionDispatcher()->GetType();
                auto type2 = collider2.GetCollisionDispatcher()->GetType();
                
                std::ostringstream ss;
                ss << "Collision between " << type1 << " and " << type2 << "is not implemented";
                return ss.str();
            }
        public:
            CollisionNotImplementedException(const Collider& collider1, const Collider& collider2) :
                runtime_error{CreateExceptionText(collider1, collider2)} {}
        };

        bool Collides(const SimplePlaneCollider& collider1, const SimplePlaneCollider& collider2) {
            throw CollisionNotImplementedException{collider1, collider2};
        }

        bool Collides(const SimplePlaneCollider& collider1, const SimpleCubeCollider& collider2) {
            auto height = collider1.position.y;
            auto planeHeight = collider2.position.y;

            return (height - collider1.size / 2) <= planeHeight && (height + collider1.size / 2) >= planeHeight;
        }

        bool Collides(const SimplePlaneCollider& collider1, const SphereCollider& collider2) {
            auto height = collider1.position.y;
            auto planeHeight = collider2.position.y;

            return (height - collider1.size / 2) <= planeHeight && (height + collider1.size / 2) >= planeHeight;
        }

        bool Collides(const SimpleCubeCollider& collider1, const SimplePlaneCollider& collider2) {
            return Collides(collider2, collider1);
        }

        bool Collides(const SimpleCubeCollider& collider1, const SimpleCubeCollider& collider2) {
            throw CollisionNotImplementedException{collider1, collider2};
        }

        bool Collides(const SimpleCubeCollider& collider1, const SphereCollider& collider2) {
            throw CollisionNotImplementedException{collider1, collider2};
        }

        bool Collides(const SphereCollider& collider1, const SimplePlaneCollider& collider2) {
            return Collides(collider2, collider1);
        }

        bool Collides(const SphereCollider& collider1, const SimpleCubeCollider& collider2) {
            throw CollisionNotImplementedException{collider1, collider2};
        }

        bool Collides(const SphereCollider& collider1, const SphereCollider& collider2) {
            return glm::length(collider1.position - collider2.position) <= (collider1.size + collider2.size) / 2;
        }
    }

bool SimplePlaneCollider::SimplePlaneDispatcher::CollidesWith(const SimplePlaneCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SimplePlaneCollider::SimplePlaneDispatcher::CollidesWith(const SimpleCubeCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SimplePlaneCollider::SimplePlaneDispatcher::CollidesWith(const SphereCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SimpleCubeCollider::SimpleCubeDispatcher::CollidesWith(const SimplePlaneCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SimpleCubeCollider::SimpleCubeDispatcher::CollidesWith(const SimpleCubeCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SimpleCubeCollider::SimpleCubeDispatcher::CollidesWith(const SphereCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SphereCollider::SphereDispatcher::CollidesWith(const SimplePlaneCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SphereCollider::SphereDispatcher::CollidesWith(const SimpleCubeCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

bool SphereCollider::SphereDispatcher::CollidesWith(const SphereCollider& other) {
    return Collision::Collides(*thisCollider, other);
}

Collider::Collider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg) : 
    position{position}, size{size}, velocity{velocity} {}

SphereCollider::SphereCollider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg) : 
    Collider{position, size, velocity, tmparg} {}

SimpleCubeCollider::SimpleCubeCollider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg) :
    Collider{position, size, velocity, tmparg} {
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
            newCollider.position.y = planeCollider.position.y + size / 2;
        }

        *this = newCollider;
    }
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
            newCollider.position = position + SmallestY(otherSphere, newPosition - position);
        }

        *this = newCollider;
    }
}



void collideAll(const std::vector<Collider*>& colliders) {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);

    for(auto collider1 : colliders) {
        auto [distance, velocity] = collider1->CalculatePositionAndVelocity();

        //auto newCollider = *collider1;
        //newCollider.position += distance;
        //newCollider.velocity = velocity;

        for(auto collider2 : colliders) {
            if(collider1 == collider2) continue;
            collider1->CollidesWith(*collider2);

        }
    }
}

} // namespace Physics
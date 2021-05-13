#include "Collision.hpp"

#include <cmath>

#include <glm/geometric.hpp>

#include <sstream>

#include <type_traits>

//#include "imgui.h"
//TODO: Figure out how to support adding to the UI from within this project

namespace Physics {

std::string NotImplementedException::CreateExceptionText(const Collider& collider1, const Collider& collider2) const {
    std::ostringstream ss;
    ss << "Collision between " << collider1.GetName() << " and " << collider2.GetName() << " is not implemented";
    return ss.str();
}

#define IMPL_COLLIDES_DEFAULT(Type1, Type2) \
    bool operator()(const Type1& collider1, const Type2& collider2) { \
        throw NotImplementedException{collider1, collider2}; \
    }

// Making this a struct with operator() allows us to use std::is_invocable
struct CollidesImpl {
    IMPL_COLLIDES_DEFAULT(SimplePlaneCollider, SimplePlaneCollider);

    bool operator()(const SimplePlaneCollider& collider1, const SimpleCubeCollider& collider2) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size / 2) <= planeHeight && (height + collider2.size / 2) >= planeHeight;
    }

    bool operator()(const SimplePlaneCollider& collider1, const SphereCollider& collider2) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size / 2) <= planeHeight && (height + collider2.size / 2) >= planeHeight;
    }

    IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SimpleCubeCollider);
    IMPL_COLLIDES_DEFAULT(SimpleCubeCollider, SphereCollider);

    bool operator()(const SphereCollider& collider1, const SphereCollider& collider2) {
        return glm::length(collider1.position - collider2.position) <= (collider1.size + collider2.size) / 2;
    }
};
#undef IMPL_COLLIDES_DEFAULT

    // Collision testing is symmetric
    // This function switches the order of arguments if necessary
    // This allows us to cut the number of required collision testing functions by half
    template<typename T, typename U>
    bool Collides(const T& t, const U& u) {
        constexpr bool invokableTU = std::is_invocable_v<CollidesImpl, T, U>;
        constexpr bool invokableUT = std::is_invocable_v<CollidesImpl, U, T>;

        static_assert(invokableTU || invokableUT);

        if constexpr(invokableTU) {
            return CollidesImpl{}(t, u);
        }
        else {
            return CollidesImpl{}(u, t);
        }
    }

#define IMPL_COLLIDES(Type1, Type2) \
template bool Collides<Type1, Type2>(const Type1&, const Type2&);

        IMPL_COLLIDES(SimplePlaneCollider, SimplePlaneCollider)
        IMPL_COLLIDES(SimplePlaneCollider, SimpleCubeCollider)
        IMPL_COLLIDES(SimplePlaneCollider, SphereCollider)

        IMPL_COLLIDES(SimpleCubeCollider, SimplePlaneCollider)
        IMPL_COLLIDES(SimpleCubeCollider, SimpleCubeCollider)
        IMPL_COLLIDES(SimpleCubeCollider, SphereCollider)

        IMPL_COLLIDES(SphereCollider, SimplePlaneCollider)
        IMPL_COLLIDES(SphereCollider, SimpleCubeCollider)
        IMPL_COLLIDES(SphereCollider, SphereCollider)

#undef IMPL_COLLIDES

Collider::Collider(glm::vec3 position, float size, glm::vec3 velocity, bool tmparg) : 
    position{position}, size{size}, velocity{velocity} {}

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
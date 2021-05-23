#include "Collision.hpp"

#include "CollisionTest.hpp"
#include "ApplyCollision.hpp"

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

template<typename T, typename U>
void ApplyCollisionToFirst(T& t, const U& u) {
    ApplyCollisionImpl(t, u);
}

#define IMPL_COLLIDES(Type1, Type2) \
template bool Collides<Type1, Type2>(const Type1&, const Type2&); \
template void ApplyCollisionToFirst<Type1, Type2>(Type1&, const Type2&);

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

Collider::Collider(glm::vec3 position, float size, glm::vec3 velocity) : 
    position{position}, size{size}, velocity{velocity} {}

constexpr std::pair<glm::vec3, glm::vec3> Collider::CalculatePositionAndVelocity() const {
    float deltaTime = static_cast<float>(Physics::timeManager->deltaTime);
    const auto& acceleration = earthGravityVector;

    auto initialVelocity = velocity;

    auto newVelocity = velocity + acceleration * deltaTime;

    auto distance = initialVelocity * deltaTime + (acceleration * (deltaTime * deltaTime / 2));

    return {position + distance, newVelocity};
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
            collider1->ApplyCollision(*collider2);

        }
    }
}

} // namespace Physics
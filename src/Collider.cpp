#include "Collider.hpp"

#include <string>

#include <fmt/core.h>

namespace Physics {
    std::string NotImplementedException::CreateExceptionText(const Collider& collider1, const Collider& collider2) {
        return fmt::format("Collision between {} and {} is not implemented", collider1.GetColliderTypeName(), collider2.GetColliderTypeName());
    }

    std::pair<glm::vec3, glm::vec3> Collider::CalculatePositionAndVelocity() const noexcept {
        auto deltaTime = static_cast<float>(Physics::timeManager->deltaTime);
        auto acceleration = hasGravity ? earthGravityVector : glm::vec3{};

        auto initialVelocity = velocity;

        auto newVelocity = velocity + acceleration * deltaTime;

        auto distance = initialVelocity * deltaTime + (acceleration * (deltaTime * deltaTime / 2));

        return {position + distance, newVelocity};
    }

    Collider::Collider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity) :
        position{position}, size{size}, velocity{velocity} {}

    Collider::Collider(glm::vec3 position, float size, glm::vec3 velocity) :
        Collider{position, glm::vec3{size}, velocity} {
    }
}
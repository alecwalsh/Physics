#include "Collider.hpp"

#include <string>

#include <fmt/core.h>

namespace Physics {
    std::string NotImplementedException::CreateExceptionText(const Collider& collider1, const Collider& collider2) {
        return fmt::format("Collision between {} and {} is not implemented", collider1.GetColliderTypeName(), collider2.GetColliderTypeName());
    }

    std::pair<glm::vec3, glm::vec3> Collider::CalculatePositionAndVelocity(glm::vec3 gravityVector, float deltaTime) const noexcept {
        auto distance = velocity * deltaTime;

        if(!hasGravity) return {position + distance, velocity};

        auto acceleration = gravityVector;

        auto newVelocity = velocity + acceleration * deltaTime;

        distance += (acceleration * (deltaTime * deltaTime / 2));

        return {position + distance, newVelocity};
    }

    Collider::Collider(glm::vec3 position, glm::vec3 size, glm::vec3 velocity) :
        position{position}, size{size}, velocity{velocity} {}

    Collider::Collider(glm::vec3 position, float size, glm::vec3 velocity) :
        Collider{position, glm::vec3{size}, velocity} {
    }
}
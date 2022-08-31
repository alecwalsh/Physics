#include "Collider.hpp"

#include <string>
#include <sstream>

namespace Physics {
    std::string NotImplementedException::CreateExceptionText(const Collider& collider1, const Collider& collider2) const {
        std::ostringstream ss;
        ss << "Collision between " << collider1.GetName() << " and " << collider2.GetName() << " is not implemented";
        return ss.str();
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
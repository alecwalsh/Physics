#include "Collision.hpp"

namespace Physics {
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
}
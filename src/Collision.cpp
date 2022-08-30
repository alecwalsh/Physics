#include "Collision.hpp"

namespace Physics {
    void ApplyVelocity(std::span<Collider*> colliders) {
        for(auto collider1 : colliders) {
            if(!collider1->hasGravity) continue;

            auto [distance, velocity] = collider1->CalculatePositionAndVelocity();

            collider1->position = distance;
            collider1->velocity = velocity;
        }
    }
}
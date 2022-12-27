#include "Collision.hpp"

#include <set>
#include <utility>

#include <glm/glm.hpp>
#include <algorithm>

#include <spdlog/spdlog.h>

namespace Physics {
    static std::set<std::pair<Collider*, Collider*>> CalculatePairs(std::span<Collider*> colliders) {
        std::set<std::pair<Collider*, Collider*>> pairs;

        spdlog::trace("Calculating colliding pairs for {} colliders", colliders.size());

        for(auto collider1 : colliders) {
            for(auto collider2 : colliders) {
                if(collider1 == collider2) continue;
                if(!collider1->SupportsCollisionWith(*collider2)) continue;

                if(collider1->CollidesWith(*collider2)) {
                    if(collider1 < collider2) {
                        pairs.emplace(collider1, collider2);
                    } else {
                        pairs.emplace(collider2, collider1);
                    }
                }
            }
        }


        spdlog::info("Found {} colliding pairs", pairs.size());

        return pairs;
    }

    static glm::vec3 CalculateImpulse(glm::vec3 relativeVel, glm::vec3 n, float restitution, float mass1, float mass2) {
        float vel = glm::dot(relativeVel, n);

        if(vel < 0) return {};

        float j = -(1 + restitution) * vel;

        j /= 1/mass1 + 1/mass2;

        return j * n;
    }

    static void ResolveCollision(std::pair<Collider*, Collider*> pair) {
        auto [collider1, collider2] = pair;

        auto result = collider1->CollidesWith(*collider2);

        if(result.collides) {
            auto n = result.normal;

            spdlog::trace("Collision between {} and {}", collider1->name, collider2->name);
            spdlog::trace("Collision normal: {}, {}, {}", n.x, n.y, n.z);
            spdlog::trace("Collision penetration: {}", result.penetration);

            float restitution = std::min(collider1->restitution, collider2->restitution);

            auto relativeVel = collider1->velocity - collider2->velocity;

            auto impulse = CalculateImpulse(relativeVel, n, restitution, collider1->mass, collider2->mass);

            collider1->velocity += 1 / collider1->mass * impulse;
            collider2->velocity -= 1 / collider2->mass * impulse;
        }
    }

    void ResolveCollisions(std::span<Collider*> colliders) {
        auto pairs = CalculatePairs(colliders);

        std::for_each(pairs.begin(), pairs.end(), ResolveCollision);
    }

    void ApplyVelocity(std::span<Collider*> colliders, glm::vec3 gravityVector, float deltaTime) {
        for(auto collider : colliders) {
            auto [distance, velocity] = collider->CalculatePositionAndVelocity(gravityVector, deltaTime);

            collider->position = distance;
            collider->velocity = velocity;
        }
    }
}
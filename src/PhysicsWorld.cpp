#include "PhysicsWorld.hpp"

#include "Collision.hpp"

namespace Physics {

void PhysicsWorld::Tick() {
    float deltaTime = (float)1 / tickRate;

    Physics::ApplyVelocity(physicsObjects, gravityVector, deltaTime);
    Physics::ResolveCollisions(physicsObjects);

    lastUpdate += deltaTime;
}

void PhysicsWorld::TickUntil() {
    while(lastUpdate < timeManager->elapsedTime) {
        Tick();
    }
}

void PhysicsWorld::AddPhysicsObject(Collider* collider) { physicsObjects.push_back(collider); }

}
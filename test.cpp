#include <iostream>

#include "Collision.hpp"

#include "config.hpp"

#include <glm/geometric.hpp>

std::ostream& operator<<(std::ostream& os, glm::vec3 vec) {
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return os;
}

double timeStep = 1 / 60.0;
int secondsToRun = 5;

void cubeCollisionTest() {
    using glm::vec3;
    std::cout << "Testing cube collision" << std::endl;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    auto tm = TimeManagerShim{elapsedTime, deltaTime};
    Physics::timeManager = &tm;

    Physics::SimpleCubeCollider cubeCollider1{vec3{}, vec3{0, 10, 0}, 1};

    Physics::SimplePlaneCollider planeCollider{0};

    float maxVelocity = 0;

    while(tm.elapsedTime < secondsToRun) {
        cubeCollider1.ApplyCollision(planeCollider);

        maxVelocity = std::max(maxVelocity, glm::length(cubeCollider1.velocity));

        Physics::timeManager->elapsedTime += timeStep;
    }

    auto correctFinalPosition = vec3{0, 0.5, 0};

    assert(cubeCollider1.position == correctFinalPosition);
    assert(maxVelocity == 13.7339916F);
}

void sphereCollisionTest() {
    using glm::vec3;

    std::cout << "Testing sphere collision" << std::endl;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    auto tm = TimeManagerShim{elapsedTime, deltaTime};
    Physics::timeManager = &tm;

    Physics::SphereCollider sphereCollider1{vec3{}, vec3{0, 40, 0}, 2};

    Physics::SphereCollider sphereCollider2{vec3{}, vec3{0, 5, 0}, 2};

    float maxVelocity = 0;

    while (tm.elapsedTime < secondsToRun) {
        sphereCollider1.ApplyCollision(sphereCollider2);

        maxVelocity = std::max(maxVelocity, glm::length(sphereCollider1.velocity));

        Physics::timeManager->elapsedTime += timeStep;
    }

    auto correctFinalPosition = vec3{0, 7, 0};

    assert(sphereCollider1.position == correctFinalPosition);
    assert(maxVelocity == 25.5059795F);
}

void collisionTest() {
    cubeCollisionTest();
    sphereCollisionTest();
}

int main() {
    std::cout << "Hello Physics" << std::endl;
    std::cout << "Version: " << Physics::PROJECT_VERSION << std::endl;

    //Physics::addToUI = [] (auto){};

    collisionTest();

    return 0;
}
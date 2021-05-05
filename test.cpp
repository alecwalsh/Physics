#include <iostream>

#include "Collision.hpp"

#include "config.hpp"

std::ostream& operator<<(std::ostream& os, glm::vec3 vec) {
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return os;
}

double timeStep = 1 / 60.0;
int secondsToRun = 5;

void cubeCollisionTest() {
    std::cout << "Testing cube collision" << std::endl;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    auto tm = TimeManagerShim{elapsedTime, deltaTime};
    Physics::timeManager = &tm;

    Physics::SimpleCubeCollider cubeCollider1{{0, 10, 0}, 1, {}};

    Physics::SimplePlaneCollider planeCollider{0};

    while (tm.elapsedTime < secondsToRun) {
        cubeCollider1.position += Physics::getTranslation(cubeCollider1, planeCollider);

        std::cout << "Velocity: " << cubeCollider1.velocity << std::endl;

        Physics::timeManager->elapsedTime += timeStep;
    }
}

void sphereCollisionTest() {
    std::cout << "Testing sphere collision" << std::endl;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    auto tm = TimeManagerShim{elapsedTime, deltaTime};
    Physics::timeManager = &tm;

    Physics::SphereCollider sphereCollider1{{0, 40, 0}, 1, {}};

    Physics::SphereCollider sphereCollider2{{0, 5, 0}, 1, {}};

    while (tm.elapsedTime < secondsToRun) {
        sphereCollider1.position += Physics::getTranslation(sphereCollider1, sphereCollider2);

        std::cout << "Velocity: " << sphereCollider1.velocity << std::endl;

        Physics::timeManager->elapsedTime += timeStep;
    }
}

int main() {
    std::cout << "Hello Physics" << std::endl;

    //Physics::addToUI = [] (auto){};

    sphereCollisionTest(); // -25.3425
    
    cubeCollisionTest(); // -13.5705

    return 0;
}
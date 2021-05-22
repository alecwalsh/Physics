#include "Collision.hpp"

#include "config.hpp"

#include <glm/geometric.hpp>

#include <iostream>

#include <gtest/gtest.h>

// Create an empty file with the same name as the executable suffixed with ".is_google_test" to make Visual Studio's Google Test support discover the tests

std::ostream& operator<<(std::ostream& os, glm::vec3 vec) {
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return os;
}

class CollisionTestsFixture : public ::testing::Test {
    double timeStep = 1 / 60.0;
    int secondsToRun = 5;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    TimeManagerShim tm{elapsedTime, deltaTime};
protected:
    float genericCollisionTest(Physics::Collider& collider1, const Physics::Collider& collider2) {
        float maxVelocity = 0;

        while(Physics::timeManager->elapsedTime < secondsToRun) {
            collider1.ApplyCollision(collider2);

            maxVelocity = std::max(maxVelocity, glm::length(collider1.velocity));

            Physics::timeManager->elapsedTime += timeStep;
        }

        return maxVelocity;
    }

    void SetUp() override {
        Physics::timeManager = &tm;
    }

    void TearDown() override {
        Physics::timeManager = nullptr;
    }
};

TEST_F(CollisionTestsFixture, cubeCollisionTest) {
    Physics::SimpleCubeCollider collider1{{0, 10, 0}, 1, {}};

    Physics::SimplePlaneCollider collider2{0};

    float maxVelocity = genericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 0.5, 0};

    ASSERT_EQ(collider1.position, correctFinalPosition);
    ASSERT_EQ(maxVelocity, 13.7339916F);
}

TEST_F(CollisionTestsFixture, sphereCollisionTest) {
    Physics::SphereCollider collider1{{0, 40, 0}, 2, {}};

    Physics::SphereCollider collider2{{0, 5, 0}, 2, {}};

    float maxVelocity = genericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 7, 0};

    ASSERT_EQ(collider1.position, correctFinalPosition);
    ASSERT_EQ(maxVelocity, 25.5059795F);
}

TEST_F(CollisionTestsFixture, collideAllTest) {
    std::vector<Physics::Collider*> colliders;

    Physics::SphereCollider sphereCollider1{{0, 40, 0}, 2, {}};
    Physics::SphereCollider sphereCollider2{{0, 5, 0}, 2, {}};
    Physics::SimplePlaneCollider planeCollider1{0};

    colliders.push_back(&sphereCollider1);
    colliders.push_back(&sphereCollider2);
    colliders.push_back(&planeCollider1);

    //collideAll(colliders);
}
#include <Physics/Collision.hpp>

#include <Physics/config.hpp>

#include <glm/geometric.hpp>

#include <iostream>

#include <gtest/gtest.h>

// Create an empty file with the same name as the executable suffixed with ".is_google_test" to make Visual Studio's Google Test support discover the tests

static std::ostream& operator<<(std::ostream& os, glm::vec3 vec) {
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
    float GenericCollisionTest(Physics::Collider& collider1, const Physics::Collider& collider2) {
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

TEST_F(CollisionTestsFixture, CubePlaneCollisionTest) {
    Physics::SimpleCubeCollider collider1{{0, 10, 0}, 1, {}};

    Physics::SimplePlaneCollider collider2{0};

    float maxVelocity = GenericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 0.5, 0};

    EXPECT_EQ(collider1.position, correctFinalPosition);
    EXPECT_EQ(maxVelocity, 13.7339916F);
}

TEST_F(CollisionTestsFixture, SphereSphereCollisionTest) {
    Physics::SphereCollider collider1{{0, 40, 0}, 2, {}};

    Physics::SphereCollider collider2{{0, 5, 0}, 2, {}};

    float maxVelocity = GenericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 7, 0};

    EXPECT_EQ(collider1.position, correctFinalPosition);
    EXPECT_EQ(maxVelocity, 25.5059795F);
}

TEST_F(CollisionTestsFixture, CollideAllTest) {
    std::vector<Physics::Collider*> colliders;

    Physics::SphereCollider sphereCollider1{{0, 40, 0}, 2, {}};
    Physics::SphereCollider sphereCollider2{{0, 5, 0}, 2, {}};
    Physics::SimplePlaneCollider planeCollider1{0};

    colliders.push_back(&sphereCollider1);
    colliders.push_back(&sphereCollider2);
    colliders.push_back(&planeCollider1);

    //collideAll(colliders);
}

Physics::SimpleCubeCollider CreateCube(glm::vec3 position, float size) {
    return {position, size, {}};
}

TEST_F(CollisionTestsFixture, CubesCollideSimpleTest) {
    EXPECT_TRUE(CreateCube({0, 10, 0}, 1).CollidesWith(CreateCube({0, 9, 0}, 1))); // Barely touching
    EXPECT_TRUE(CreateCube({0, 10, 0}, 1).CollidesWith(CreateCube({0, 10, 0}, 1))); // Identical cubes

    EXPECT_FALSE(CreateCube({0, 10, 0}, 1).CollidesWith(CreateCube({0, 9, 0}, 0.99)));
    EXPECT_FALSE(CreateCube({0, 10, 0}, 1).CollidesWith(CreateCube({0, 8, 0}, 1)));

    EXPECT_TRUE(CreateCube({0, 10, 0}, 1).CollidesWith(CreateCube({0, 10.75f, 0}, 1)));
    EXPECT_TRUE(CreateCube({0, 10, 0}, 2).CollidesWith(CreateCube({0, 10.5, 0}, 0.25))); // One cube inside another
}
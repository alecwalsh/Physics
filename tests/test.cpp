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
    EXPECT_EQ(maxVelocity, 13.5704918F);
}

TEST_F(CollisionTestsFixture, SphereSphereCollisionTest) {
    Physics::SphereCollider collider1{{0, 40, 0}, 2, {}};

    Physics::SphereCollider collider2{{0, 5, 0}, 2, {}};

    float maxVelocity = GenericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 7, 0};

    EXPECT_EQ(collider1.position, correctFinalPosition);
    EXPECT_EQ(maxVelocity, 25.3424797F);
}

TEST_F(CollisionTestsFixture, SpherePlaneCollisionTest) {
    Physics::SphereCollider collider1{{0, 40, 0}, 2, {}};

    Physics::SimplePlaneCollider collider2{0};

    float maxVelocity = GenericCollisionTest(collider1, collider2);

    glm::vec3 correctFinalPosition = {0, 1, 0};

    EXPECT_EQ(collider1.position, correctFinalPosition);
    EXPECT_EQ(maxVelocity, 27.6314774F);
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

Physics::SimpleCubeCollider CreateCube(glm::vec3 position, glm::vec3 size) {
    return {position, size, {}};
}

Physics::SimpleCubeCollider CreateCube(glm::vec3 position, float size) {
    return CreateCube(position, glm::vec3{size});
}

bool Collides(const Physics::SimpleCubeCollider& cube1, const Physics::SimpleCubeCollider& cube2) {
    return cube1.CollidesWith(cube2);
}

struct CubeNoVelocity {
    glm::vec3 position;
    glm::vec3 size;

    CubeNoVelocity(glm::vec3 position, glm::vec3 size) : position{position}, size{size} {}
    CubeNoVelocity(glm::vec3 position, float size) : CubeNoVelocity{position, glm::vec3{size}} {}
};

bool Collides(CubeNoVelocity cube1, CubeNoVelocity cube2) {
    return Collides(CreateCube(cube1.position, cube1.size), CreateCube(cube2.position, cube2.size));
}

TEST_F(CollisionTestsFixture, CubesCollideSimpleTest) {
    // Test cubes
    EXPECT_FALSE(Collides( // Barely touching
        {{0, 10, 0}, 1},
        {{0, 9, 0}, 1}
    ));
    EXPECT_TRUE(Collides( // Identical cubes
        {{0, 10, 0}, 1},
        {{0, 10, 0}, 1}
    ));

    EXPECT_TRUE(Collides(
        {{0, 10, 0}, 1},
        {{0, 10.75f, 0}, 1}
    ));
    EXPECT_TRUE(Collides( // One cube inside another
        {{0, 10, 0}, 2},
        {{0, 10.5, 0}, 0.25}
    ));

    // Now test cuboids
    EXPECT_FALSE(Collides( // Barely touching x axis
        {{0, 0, 0}, {3, 2, 1}},
        {{2, 0, 0}, {1, 2, 3}}
    ));
    EXPECT_FALSE(Collides( // Barely touching y axis
        {{0, 0, 0}, {3, 2, 1}},
        {{0, -2, 0}, {1, 2, 3}}
    ));
    EXPECT_FALSE(Collides( // Barely touching z axis
        {{0, 0, 0}, {3, 2, 1}},
        {{0, 0, -2}, {1, 2, 3}}
    ));

    EXPECT_TRUE(Collides( // Identical cuboids
        {{0, 10, 0}, {3, 2, 1}},
        {{0, 10, 0}, {3, 2, 1}}
    ));

    EXPECT_FALSE(Collides(
        {{0, 10, 0}, {3, 4, 5}},
        {{0.5, 12.50, -2}, {1, 0.99, 1}}
    ));
    EXPECT_FALSE(Collides(
        {{0, 10, 0}, {3, 4, 5}},
        {{0.5, 13, -2}, 1}
    ));

    EXPECT_TRUE(Collides(
        {{0, 10, 0}, {3, 4, 5}},
        {{0.5, 11, -2}, 1}
    ));
    EXPECT_TRUE(Collides( // One cuboid inside another
        {{0, 10, 0}, {3, 4, 5}},
        {{0.5, 11, -2}, 0.25}
    ));
}
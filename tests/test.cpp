#include <Physics/Collision.hpp>
#include <Physics/PhysicsWorld.hpp>

#include <Physics/config.hpp>

#include <glm/geometric.hpp>

#include <iostream>
#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

// Create an empty file with the same name as the executable suffixed with ".is_google_test" to make Visual Studio's Google Test support discover the tests

namespace glm {
static std::ostream& operator<<(std::ostream& os, glm::vec3 vec) {
    os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return os;
}
}

class CollisionTestsFixture : public ::testing::Test {
    double timeStep = 1 / 60.0;
    int secondsToRun = 5;

    double elapsedTime = 0;
    double deltaTime = timeStep;

    TimeManagerShim tm{elapsedTime, deltaTime};

    Physics::PhysicsWorld physicsWorld;
protected:
    void SetUp() override {
        physicsWorld.timeManager = &tm;
    }
};

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

TEST_F(CollisionTestsFixture, SupportCollisionTest) {
    Physics::SimpleCubeCollider cube{{1,2,3}, 1, {0,0,0}};
    Physics::SphereCollider sphere{{1,2,3}, 1, {0,0,0}};
    Physics::SimplePlaneCollider plane{1};

    EXPECT_FALSE(plane.SupportsCollisionWith(plane));
    EXPECT_TRUE(plane.SupportsCollisionWith(cube));
    EXPECT_TRUE(plane.SupportsCollisionWith(sphere));

    EXPECT_TRUE(cube.SupportsCollisionWith(plane));
    EXPECT_TRUE(cube.SupportsCollisionWith(cube));
    EXPECT_FALSE(cube.SupportsCollisionWith(sphere));

    EXPECT_TRUE(sphere.SupportsCollisionWith(plane));
    EXPECT_FALSE(sphere.SupportsCollisionWith(cube));
    EXPECT_TRUE(sphere.SupportsCollisionWith(sphere));
}

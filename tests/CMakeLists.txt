find_package(GTest CONFIG REQUIRED)

add_executable(glfwogltest2_physics_test test.cpp)

target_link_libraries(glfwogltest2_physics_test PRIVATE glfwogltest2_physics GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(glfwogltest2_physics_test)
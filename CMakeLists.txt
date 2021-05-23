cmake_minimum_required(VERSION 3.8.0)

project(glfwogltest2_physics VERSION 0.0.0.1)

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(tests)
endif()
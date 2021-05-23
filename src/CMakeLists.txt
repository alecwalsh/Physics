file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "../include/*.hpp")

set(SOURCES ${SOURCES} ${HEADERS})

add_library(glfwogltest2_physics ${SOURCES})

find_package(glm CONFIG REQUIRED)

if(MSVC)
    target_compile_definitions(glfwogltest2_physics PRIVATE NOMINMAX)
    target_compile_options(glfwogltest2_physics PRIVATE "/W4")
    target_compile_options(glfwogltest2_physics PRIVATE "/Zc:__cplusplus") # Use the correct value for the __cplusplus macro
else()
    target_compile_options(glfwogltest2_physics PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(glfwogltest2_physics PUBLIC c_std_11 cxx_std_17)

target_include_directories(glfwogltest2_physics PUBLIC "../include")

target_link_libraries(glfwogltest2_physics PUBLIC glm::glm)

if(UNIX)
    target_link_libraries(glfwogltest2_physics PRIVATE stdc++fs) # Need to link with stdc++fs for std::filesystem support on some older compilers
endif()

#TODO: Use correct visibility in this file, instead of just PUBLIC
configure_file(../include/config.hpp.in config.hpp)
target_include_directories(glfwogltest2_physics PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2_physics PUBLIC DEBUG)
endif()
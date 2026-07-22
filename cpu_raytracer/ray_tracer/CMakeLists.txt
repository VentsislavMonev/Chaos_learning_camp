cmake_minimum_required(VERSION 3.15)
project(RayTracer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Automatically grab all .cpp files in src/
file(GLOB SOURCES "src/*.cpp")

add_executable(raytracer ${SOURCES})

add_executable(homework homeworks/homework4.cpp)
target_include_directories(homework PRIVATE include)

# So #include "vec3.hpp" works without relative paths
target_include_directories(raytracer PRIVATE include)

# Optional: multithreading support
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracer PUBLIC OpenMP::OpenMP_CXX)
endif()

target_compile_options(raytracer PRIVATE
    $<$<CONFIG:Release>:-O3>
)
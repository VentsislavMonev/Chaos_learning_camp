
cmake_minimum_required(VERSION 3.15)
project(RayTracer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Automatically grab all .cpp files in src/
file(GLOB SOURCES "src/*.cpp")

add_executable(raytracer
    ${SOURCES}
    external/stb/stb_image.cpp
)

# Include directories
target_include_directories(raytracer PRIVATE
    include
    external/stb
    external/rapidjson
)

if(MSVC)
    target_compile_options(raytracer PRIVATE
        $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(raytracer PRIVATE
        $<$<CONFIG:Release>:-O3>
    )
endif()

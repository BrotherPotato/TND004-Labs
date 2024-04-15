cmake_minimum_required(VERSION 3.21.0...3.29)
project(TND004-Lab-3-part1 VERSION 1.0.0 DESCRIPTION "TND004 Lab 3: part1" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab3-part1 
    include/particlesystem/collisionsystem.h 
    include/particlesystem/event.h 
    include/particlesystem/particle.h 
    include/particlesystem/priorityqueue.h 
    include/rendering/window.h 
    src/particlesystem/collisionsystem.cpp 
    src/particlesystem/event.cpp
    src/particlesystem/particle.cpp 
    src/rendering/window.cpp
    src/lab3.cpp 
)

target_include_directories(lab3-part1 PUBLIC "include")
target_compile_options(lab3-part1 PUBLIC 
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra>
)

# External libraries
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(lab3-part1 PUBLIC glm::glm fmt::fmt glad::glad glfw)

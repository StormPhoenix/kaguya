cmake_minimum_required(VERSION 3.15)
project(kaguya)

set(CMAKE_CXX_STANDARD 11)

include_directories(./include)

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/material/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/accumulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/meta/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp)

add_executable(kaguya ${sources})
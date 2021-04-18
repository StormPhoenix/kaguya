cmake_minimum_required(VERSION 3.15)
project(kaguya)

set(CMAKE_CXX_STANDARD 14)

include_directories(./include)
include_directories(/usr/local/include)

message(${CMAKE_SYSTEM_NAME})
message(${CMAKE_CXX_COMPILER_ID})
message(${CMAKE_C_COMPILER_ID})

INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckCXXSourceRuns)

## Cache alignas
CHECK_CXX_SOURCE_COMPILES("
int main() {
    int a = alignof(double);
}" HAVE_ALIGNOF)
if (HAVE_ALIGNOF)
    add_definitions(-D KAGUYA_HAVE_ALIGNOF)
endif (HAVE_ALIGNOF)

CHECK_CXX_SOURCE_COMPILES("
#include <malloc.h>
int main() {
    void *p = _aligned_malloc(1024, 32);
} " WINDOWS_MALLOC_ALIGNED)
if (WINDOWS_MALLOC_ALIGNED)
    add_definitions(-D KAGUYA_WINDOWS_MALLOC_ALIGNED)
endif (WINDOWS_MALLOC_ALIGNED)

CHECK_CXX_SOURCE_COMPILES("
#include <stdlib.h>
int main() {
    void *p;
    posix_memalign(&p, 32, 1024);
} " POSIX_MALLOC_ALIGNED)
if (POSIX_MALLOC_ALIGNED)
    add_definitions(-D KAGUYA_POSIX_MALLOC_ALIGNED)
endif (POSIX_MALLOC_ALIGNED)

message("${CMAKE_CXX_COMPILER}")
message("${APPLE}")

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bsdf/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bssrdf/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/light/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/phase/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/medium/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/material/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/sampler/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/parallel/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/accumulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/importer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/importer/xml/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/meta/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/utils/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/bdpt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pugixml/*.cpp)

add_executable(kaguya ${sources})
target_link_libraries(kaguya pthread)

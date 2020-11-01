cmake_minimum_required(VERSION 3.15)
project(kaguya)

set(CMAKE_CXX_STANDARD 14)

include_directories(./include)
include_directories(/usr/local/include)

message(${CMAKE_SYSTEM_NAME})
message(${CMAKE_CXX_COMPILER_ID})
message(${CMAKE_C_COMPILER_ID})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(ASSIMP_INCLUDE /usr/include)
    set(ASSIMP_LINK /usr/lib/x86_64-linux-gnu/libassimp.so)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(ASSIMP_INCLUDE /usr/local/include)
    set(ASSIMP_LINK /usr/local/Cellar/assimp/5.0.1/lib/libassimp.5.0.0.dylib)
    set(OPENMP_LIBRARIES "/usr/local/Cellar/libomp/10.0.1/lib")
    set(OPENMP_INCLUDES "/usr/local/Cellar/libomp/10.0.1/include")
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

include_directories(${ASSIMP_INCLUDE})
link_libraries(${ASSIMP_LINK})

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
OPTION(USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

# Find OpenMP
if (APPLE AND USE_OpenMP)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        #注意以上需要增加-Xpreprocessor 以及不能直接-llibomp 在这里不需要前缀lib只需要-lomp即可，下面相似的地方也是同个道理
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif ()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif ()
endif ()

if (USE_OpenMP)
    find_package(OpenMP REQUIRED)
endif (USE_OpenMP)

if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (OPENMP_FOUND)


file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bsdf/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/light/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/material/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/accumulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/meta/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/utils/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/bdpt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp)

add_executable(kaguya ${sources})
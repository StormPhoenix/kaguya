cmake_minimum_required(VERSION 3.15)
project(kaguya)

set(CMAKE_CXX_STANDARD 14)

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
    add_definitions(-D RENDER_WINDOWS_MALLOC_ALIGNED)
endif (WINDOWS_MALLOC_ALIGNED)

CHECK_CXX_SOURCE_COMPILES("
#include <stdlib.h>
int main() {
    void *p;
    posix_memalign(&p, 32, 1024);
} " POSIX_MALLOC_ALIGNED)
if (POSIX_MALLOC_ALIGNED)
    add_definitions(-D RENDER_POSIX_MALLOC_ALIGNED)
endif (POSIX_MALLOC_ALIGNED)

message("${CMAKE_CXX_COMPILER}")
message("${APPLE}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMake/)

find_package(CUDA REQUIRED)
if (CUDA_FOUND)
    enable_language(CUDA)
    set(CUDA_LINK_LIBRARY ${CUDA_LIBRARIES})
    set(CUDA_INCLUDE_DIRECTORY ${CUDA_INCLUDE_DIRS})
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-gencode arch=compute_61，code=sm_61;)
    message(STATUS "Found CUDA")
endif (CUDA_FOUND)

find_package(OptiX REQUIRED)
if (OptiX_FOUND)
    set(OPTIX_INCLUDE_DIRECTORY ${OptiX_INCLUDE_DIR})
    message(STATUS "Found OptiX")
endif (OptiX_FOUND)

get_filename_component(cuda_compiler_bin ${CMAKE_CUDA_COMPILER} DIRECTORY)
find_program(BIN2C
        NAMES bin2c
        PATHS ${cuda_compiler_bin}
        NO_DEFAULT_PATH)
if (BIN2C)
    message(STATUS "Found program bin2c")
elseif (BIN2C)
    message(FATAL_ERROR "BIN2C not found")
endif (BIN2C)

set(RENDER_GPU_MODE_ENABLED false)
#if (CUDA_FOUND AND CUDA_FOUND)
#    set(RENDER_GPU_MODE_ENABLED true)
#    add_definitions(-D _RENDER_GPU_MODE_)
#endif (CUDA_FOUND AND CUDA_FOUND)

set(PROJECT_INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include)

set(RENDER_UTILS_SOURCES
        src/kaguya/utils/memory/MemoryAllocator.cpp
        src/kaguya/utils/memory/HostResource.cpp
        src/kaguya/utils/FileReader.cpp
        src/kaguya/utils/ImageIO.cpp
        src/kaguya/utils/MemoryArena.cpp
        src/kaguya/utils/ObjLoader.cpp
        src/kaguya/utils/VisibilityTester.cpp)

set(RENDER_SAMPLER_SOURCES
        src/kaguya/sampler/DefaultSampler.cpp
        src/kaguya/sampler/HaltonSampler.cpp
        src/kaguya/sampler/IndependentSampler.cpp
        src/kaguya/sampler/Sampler.cpp
        src/kaguya/sampler/SimpleHaltonSampler.cpp)

set(RENDER_GPU_SOURCES "")
if (RENDER_GPU_MODE_ENABLED)
    set(RENDER_GPU_SOURCES
            src/kaguya/utils/memory/CUDAResource.cpp)

    set_source_files_properties(
            include/kaguya/sampler/RNG.h
            include/kaguya/utils/TaggedPointer.h

            include/kaguya/sampler/IndependentSampler.h
            src/kaguya/sampler/IndependentSampler.cpp

            PROPERTIES LANGUAGE CUDA)
endif (RENDER_GPU_MODE_ENABLED)

file(GLOB RENDER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bsdf/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bsdf/fresnel/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bsdf/microfacet/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/bssrdf/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/light/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/phase/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/core/medium/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/material/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/parallel/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/accumulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/importer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/importer/xml/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/meta/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/bdpt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pugixml/*.cpp)

add_executable(${PROJECT_NAME}
        ${RENDER_UTILS_SOURCES}
        ${RENDER_SAMPLER_SOURCES}

        ${RENDER_SOURCES}
        ${RENDER_GPU_SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CUDA_INCLUDE_DIRECTORY}
        ${PROJECT_INCLUDE_DIRECTORY})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${CUDA_LINK_LIBRARY})

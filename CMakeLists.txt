cmake_minimum_required(VERSION 3.15)
project(Tunan)

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

find_package(CUDA)
if (CUDA_FOUND)
    enable_language(CUDA)
    set(CUDA_LINK_LIBRARY ${CUDA_LIBRARIES})
    set(CUDA_INCLUDE_DIRECTORY ${CUDA_INCLUDE_DIRS})
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-gencode arch=compute_61，code=sm_61;)
    message(STATUS "Found CUDA: ${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR}")
    #    add_definitions(-D CUDA_VERSION)
endif (CUDA_FOUND)

find_package(OptiX)
if (OptiX_FOUND)
    set(OPTIX_INCLUDE_DIRECTORY ${OptiX_INCLUDE_DIR})
    message(STATUS "Found OptiX")
endif (OptiX_FOUND)

#option(START_CUDA "option for cuda" ON)
option(START_CUDA "option for cuda" OFF)

set(RENDER_GPU_MODE_ENABLED false)
if (CUDA_FOUND AND CUDA_FOUND AND START_CUDA)
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

    set(RENDER_GPU_MODE_ENABLED true)
    add_definitions(-D __RENDER_GPU_MODE__)

    message(STATUS "Compile in GPU Mode.")
endif (CUDA_FOUND AND CUDA_FOUND AND START_CUDA)

set(PROJECT_INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include)

set(RENDER_UTILS_SOURCES
        src/kaguya/utils/memory/MemoryAllocator.cpp
        src/kaguya/utils/memory/HostResource.cpp
        src/kaguya/utils/FileReader.cpp
        src/kaguya/utils/ImageIO.cpp
        src/kaguya/utils/ObjLoader.cpp
        src/kaguya/utils/VisibilityTester.cpp)

set(RENDER_SAMPLER_SRCS
        #        src/kaguya/sampler/HaltonSampler.cpp
        include/kaguya/sampler/RNG.h
        src/kaguya/sampler/Sampler.cpp
        src/kaguya/sampler/IndependentSampler.cpp
        src/kaguya/sampler/SimpleHaltonSampler.cpp)

set(RENDER_MATERIAL_SRCS
        src/kaguya/material/Material.cpp
        src/kaguya/material/Lambertian.cpp
        src/kaguya/material/Dielectric.cpp
        src/kaguya/material/Metal.cpp
        src/kaguya/material/Mirror.cpp
        src/kaguya/material/PatinaMaterial.cpp
        src/kaguya/material/PlasticMaterial.cpp
        src/kaguya/material/SubsurfaceMaterial.cpp
        src/kaguya/material/OrenNayar.cpp)

set(RENDER_MATERIAL_HEADERS
        include/kaguya/material/Material.h
        include/kaguya/material/Lambertian.h
        include/kaguya/material/Dielectric.h
        include/kaguya/material/Metal.h
        include/kaguya/material/Mirror.h
        include/kaguya/material/PatinaMaterial.h
        include/kaguya/material/PlasticMaterial.h
        include/kaguya/material/SubsurfaceMaterial.h
        include/kaguya/material/OrenNayar.h)

set(RENDER_SCENE_SOURCES
        src/kaguya/scene/accumulation/AABB.cpp
        src/kaguya/scene/accumulation/BVH.cpp
        src/kaguya/scene/meta/Cube.cpp
        src/kaguya/scene/meta/Geometry.cpp
        src/kaguya/scene/meta/Sphere.cpp
        src/kaguya/scene/meta/Triangle.cpp
        src/kaguya/scene/meta/TriangleMesh.cpp
        src/kaguya/scene/SimpleScene.cpp
        src/kaguya/scene/Shape.cpp
        src/kaguya/scene/Scene.cpp
        src/kaguya/scene/importer/xml/XmlParseInfo.cpp
        src/kaguya/scene/importer/xml/XmlSceneImporter.cpp)

set(RENDER_SCENE_HEADERS
        ${PROJECT_INCLUDE_DIRECTORY})

set(RENDER_CPU_SRC "")
set(RENDER_GPU_SRC "")

if (RENDER_GPU_MODE_ENABLED)
    set(RENDER_GPU_SRC
            src/kaguya/utils/memory/CUDAResource.cpp)

    set(RENDER_GPU_HEADERS
            ${CUDA_INCLUDE_DIRECTORY}
            ${OPTIX_INCLUDE_DIRECTORY})

    set(RENDER_SCENE_SOURCES
            ${RENDER_SCENE_SOURCES}
            src/kaguya/scene/importer/xml/XmlOptiXSceneImporter.cpp
            src/kaguya/scene/OptiXScene.cpp)
    set(RENDER_SCENE_HEADERS
            ${RENDER_SCENE_HEADERS}
            ${CUDA_INCLUDE_DIRECTORY}
            ${OPTIX_INCLUDE_DIRECTORY})

    set_source_files_properties(
            ${RENDER_SAMPLER_SRCS}

            include/kaguya/utils/TaggedPointer.h

            ${RENDER_MATERIAL_HEADERS}
            ${RENDER_MATERIAL_SRCS}

            PROPERTIES LANGUAGE CUDA)
endif (RENDER_GPU_MODE_ENABLED)

file(GLOB RENDER_SRCS
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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/parallel/*.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/bdpt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pm/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/pt/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pugixml/*.cpp)

if (RENDER_GPU_MODE_ENABLED)
    set(RENDER_SRCS
            ${RENDER_SRCS}
            ${RENDER_GPU_SRC})
else (RENDER_GPU_MODE_ENABLED)
    set(RENDER_SRCS
            ${RENDER_SRCS}
            ${RENDER_CPU_SRC})
endif (RENDER_GPU_MODE_ENABLED)

add_library(render_scene_lib
        ${RENDER_SCENE_SOURCES})
target_include_directories(render_scene_lib
        PRIVATE
        ${RENDER_SCENE_HEADERS})

add_library(render_sampler_lib
        ${RENDER_SAMPLER_SRCS})
target_include_directories(render_sampler_lib
        PRIVATE
        ${PROJECT_INCLUDE_DIRECTORY})

add_library(render_utils_lib
        ${RENDER_UTILS_SOURCES})
target_include_directories(render_utils_lib
        PRIVATE
        ${PROJECT_INCLUDE_DIRECTORY})

add_library(render_material_lib
        ${RENDER_MATERIAL_SRCS})
target_include_directories(render_material_lib
        PRIVATE
        ${RENDER_MATERIAL_HEADERS}
        ${PROJECT_INCLUDE_DIRECTORY})

add_executable(${PROJECT_NAME}
        ${RENDER_SRCS})
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${RENDER_GPU_HEADERS}
        ${PROJECT_INCLUDE_DIRECTORY})
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        render_scene_lib
        render_material_lib
        render_sampler_lib
        render_utils_lib
        ${CUDA_LINK_LIBRARY})

cmake_minimum_required(VERSION 3.15)
project(kaguya)

set(CMAKE_CXX_STANDARD 11)

include_directories(./include /usr/local/include)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(OPENMP_LIBRARIES "/usr/local/Cellar/libomp/10.0.1/lib")
set(OPENMP_INCLUDES "/usr/local/Cellar/libomp/10.0.1/include")

message("${CMAKE_CXX_COMPILER}")

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

# Find OpenMP
if(APPLE AND USE_OpenMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        #注意以上需要增加-Xpreprocessor 以及不能直接-llibomp 在这里不需要前缀lib只需要-lomp即可，下面相似的地方也是同个道理
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -lomp -Wno-unused-command-line-argument")
        set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

if(USE_OpenMP)
    find_package(OpenMP REQUIRED)
endif(USE_OpenMP)


if (OPENMP_FOUND)
    include_directories("${OPENMP_INCLUDES}")
    link_directories("${OPENMP_LIBRARIES}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

set(ASSIMP_LINK /usr/local/Cellar/assimp/5.0.1/lib/libassimp.5.0.0.dylib)
link_libraries(${ASSIMP_LINK})

file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/material/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/math/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/accumulation/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/meta/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/scene/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/utils/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/kaguya/tracer/*.cpp)

add_executable(kaguya ${sources})
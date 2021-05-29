//
// Created by StormPhoenix on 2021/5/28.
//

#ifndef KAGUYA_UTILS_H
#define KAGUYA_UTILS_H

#include <kaguya/common.h>

#ifdef _RENDER_GPU_MODE_

#include <cuda.h>
#include <cuda_runtime_api.h>

#define CUDA_CHECK(exp) \
    if (exp != cudaSuccess) { \
        cudaError_t error = cudaGetLastError(); \
        ASSERT(false, "CUDA error: " + std::string(cudaGetErrorString(error))); \
    } else {}

#else

#define CUDA_CHECK(exp) ASSERT(false, "CUDA error: unimplemented");

#endif

#endif //KAGUYA_UTILS_H

//
// Created by StormPhoenix on 2021/5/28.
//

#include <kaguya/utils/memory/CUDAResource.h>
#include <kaguya/gpu/utils.h>

#include <cuda.h>
#include <cuda_runtime.h>

namespace RENDER_NAMESPACE {
    namespace memory {
        void *CUDAResource::allocateAlignedMemory(size_t bytes, size_t alignBytes) {
            void *ptr;
            CUDA_CHECK(cudaMallocManaged(&ptr, bytes));
            return ptr;
        }

        void CUDAResource::freeAlignedMemory(void *ptr) {
            CUDA_CHECK(cudaFree(ptr));
        }
    }
}
//
// Created by StormPhoenix on 2021/5/28.
//

#include <kaguya/utils/memory/HostResource.h>

#if defined(RENDER_WINDOWS_MALLOC_ALIGNED)
#include <malloc.h>
#elif defined(RENDER_POSIX_MALLOC_ALIGNED)
#include <stdlib.h>
#endif

namespace RENDER_NAMESPACE {
    namespace memory {
        void *HostResource::allocateAlignedMemory(size_t bytes, size_t alignBytes) {
#if defined(RENDER_WINDOWS_MALLOC_ALIGNED)
            return _aligned_malloc(bytes, alignBytes);
#elif defined(RENDER_POSIX_MALLOC_ALIGNED)
            void *ptr;
            if (posix_memalign(&ptr, alignBytes, bytes) != 0) {
                ptr = nullptr;
            }
            return ptr;
#else
            return memalign(alignBytes, bytes);
#endif
        }

        void HostResource::freeAlignedMemory(void *ptr) {
            if (ptr == nullptr) {
                return;
            }
#if defined(RENDER_WINDOWS_MALLOC_ALIGNED)
            _aligned_free(ptr);
#else
            free(ptr);
#endif
        }
    }
}
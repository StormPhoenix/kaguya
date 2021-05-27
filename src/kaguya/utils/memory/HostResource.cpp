//
// Created by StormPhoenix on 2021/5/28.
//

#include <kaguya/utils/memory/HostResource.h>

#if defined(KAGUYA_WINDOWS_MALLOC_ALIGNED)
#include <malloc.h>
#elif defined(KAGUYA_POSIX_MALLOC_ALIGNED)
#include <stdlib.h>
#endif

namespace RENDER_NAMESPACE {
    namespace memory {
        void *HostResource::allocateAlignedMemory(size_t bytes, size_t alignBytes) {
#if defined(KAGUYA_WINDOWS_MALLOC_ALIGNED)
            return _aligned_malloc(bytes, alignBytes);
#elif defined(KAGUYA_POSIX_MALLOC_ALIGNED)
            void *ptr;
            if (posix_memalign(&ptr, alignBytes, bytes) != 0) {
                ptr = nullptr;
            }
            return ptr;
#else
            return memalign(alignBytes, bytes);
#endif
        }
    }
}
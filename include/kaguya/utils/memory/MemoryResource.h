//
// Created by StormPhoenix on 2021/5/28.
//

#ifndef KAGUYA_MEMORYRESOURCE_H
#define KAGUYA_MEMORYRESOURCE_H

#include <kaguya/common.h>

namespace RENDER_NAMESPACE {
    namespace memory {
        class MemoryResource {
        public:
            virtual void *allocateAlignedMemory(size_t bytes, size_t alignBytes) = 0;

            virtual void freeAlignedMemory(void *ptr) = 0;
        };
    }
}

#endif //KAGUYA_MEMORYRESOURCE_H

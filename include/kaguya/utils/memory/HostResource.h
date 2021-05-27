//
// Created by StormPhoenix on 2021/5/28.
//

#ifndef KAGUYA_HOSTRESOURCE_H
#define KAGUYA_HOSTRESOURCE_H

#include <kaguya/common.h>
#include <kaguya/utils/memory/MemoryResource.h>

namespace RENDER_NAMESPACE {
    namespace memory {
        class HostResource : public MemoryResource {
        public:
            virtual void *allocateAlignedMemory(size_t bytes, size_t alignBytes) override;

            virtual void freeAlignedMemory(void *ptr) override;
        };
    }
}

#endif //KAGUYA_HOSTRESOURCE_H

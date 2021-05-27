//
// Created by StormPhoenix on 2021/5/27.
//

#ifndef KAGUYA_MEMORYALLOCATOR_H
#define KAGUYA_MEMORYALLOCATOR_H

#include <kaguya/common.h>
#include <kaguya/utils/memory/MemoryResource.h>

#include <list>
#include <cstddef>
#include <cstdint>

namespace RENDER_NAMESPACE {
    namespace memory {
        class MemoryAllocator {
        public:
            MemoryAllocator();

            MemoryAllocator(MemoryResource *resource);

            void *allocate(size_t bytes, size_t alignBytes);

            template<typename T>
            T *allocateObject(size_t count) {
                return static_cast<T *>(allocate(count * sizeof(T), sizeof(T)));
            }

            void reset();

            ~MemoryAllocator();

        private:
            MemoryAllocator(const MemoryAllocator &arena) = delete;

            MemoryAllocator &operator+(const MemoryAllocator &arena) = delete;

        private:
            MemoryResource *_resource;
            size_t _defaultBlockSize;

            uint8_t *_currentBlock;
            size_t _blockOffset;
            size_t _allocatedBlockSize;

            std::list<std::pair<size_t, uint8_t *>> _usedBlocks;
            std::list<std::pair<size_t, uint8_t *>> _availableBlocks;
        };
    }
}

#endif //KAGUYA_MEMORYALLOCATOR_H

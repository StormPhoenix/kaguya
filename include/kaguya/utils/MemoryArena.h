//
// Created by Storm Phoenix on 2020/10/20.
//

#ifndef KAGUYA_MEMORYARENA_H
#define KAGUYA_MEMORYARENA_H

// 内存 alloc、free 管理类
#include <list>
#include <cstddef>
#include <cstdint>
#include <kaguya/common.h>

#ifndef RENDER_CACHE_LINE_SIZE
// cache line size 默认为 64 字节
#define RENDER_CACHE_LINE_SIZE 64
#endif

// 内存分配函数
// TODO delete
#define ALLOC(memoryArena, type) new (memoryArena.alloc(sizeof(type))) type

void *allocAlignedMemory(size_t bytes);

void freeAlignedMemory(void *ptr);

template<typename T>
T *allocAlignedMemory(size_t count) {
    return (T *) allocAlignedMemory(count * sizeof(T));
}

namespace RENDER_NAMESPACE {
    namespace memory {

        // 计算字节对其量
#ifdef KAGUYA_HAVE_ALIGNOF
        const int alignBytes = alignof(float);
#else
        const int alignBytes = 16;
#endif

        class
#ifdef KAGUYA_CACHE_ALIGNAS
            alignas(RENDER_CACHE_LINE_SIZE)
#endif
        MemoryArena {
        public:
            MemoryArena(const size_t blockSize = 262144);

            ~MemoryArena();

            /**
             * 分配内存
             * @param size
             */
            void *alloc(size_t bytes);

            template<typename T>
            T *alloc(size_t count, bool initialize) {
                T *ret = (T *) alloc(sizeof(T) * count);
                if (initialize) {
                    for (int offset = 0; offset < count; offset++) {
                        new(&ret[offset])T();
                    }
                }
                return ret;
            }

            /**
             * 清空所有已分配内存
             */
            void clean();

        private:
            // 禁止复制操作
            MemoryArena(const MemoryArena &arena) = delete;

            // 禁止复制操作
            MemoryArena &operator+(const MemoryArena &arena) = delete;

        private:
            // 每次分配的块大小
            const size_t _blockSize;
            // 实际分配的块大小
            size_t _allocatedBlockSize;
            // 下一次可分配内存的指针偏移
            size_t _currentOffset;
            // 当前分配的数据块
            uint8_t *_block = nullptr;
            // 当前正在被使用的数据块
            std::list<std::pair<size_t, uint8_t *>> _usedBlocks;
            // 当前空闲数据块
            std::list<std::pair<size_t, uint8_t *>> _availableBlocks;
        };

    }
}

#endif //KAGUYA_MEMORYARENA_H

//
// Created by Storm Phoenix on 2020/10/20.
//

#include <kaguya/utils/MemoryArena.h>
#include <kaguya/common.h>

#if defined(KAGUYA_WINDOWS_MALLOC_ALIGNED)
#include <malloc.h>
#elif defined(KAGUYA_POSIX_MALLOC_ALIGNED)

#include <stdlib.h>

#endif

void *allocAlignedMemory(size_t bytes) {
#if defined(KAGUYA_WINDOWS_MALLOC_ALIGNED)
    return _aligned_malloc(bytes, KAGUYA_CACHE_LINE_SIZE);
#elif defined(KAGUYA_POSIX_MALLOC_ALIGNED)
    void *ptr;
    if (posix_memalign(&ptr, KAGUYA_CACHE_LINE_SIZE, bytes) != 0) ptr = nullptr;
    return ptr;
#else
    return memalign(KAGUYA_CACHE_LINE_SIZE, bytes);
#endif
}

void freeAlignedMemory(void *ptr) {
    if (ptr == nullptr) {
        return;
    }
#if defined(KAGUYA_WINDOWS_MALLOC_ALIGNED)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

namespace RENDER_NAMESPACE {
    namespace memory {

        MemoryArena::MemoryArena(const size_t blockSize) :
                _blockSize(blockSize) {
            _allocatedBlockSize = 0;
            _currentOffset = 0;
            _block = nullptr;
        }

        MemoryArena::~MemoryArena() {
            // 释放所有分配的内存
            freeAlignedMemory(_block);
            for (auto iter = _usedBlocks.begin(); iter != _usedBlocks.end(); iter++) {
                freeAlignedMemory(iter->second);
            }

            for (auto iter = _availableBlocks.begin(); iter != _availableBlocks.end(); iter++) {
                freeAlignedMemory(iter->second);
            }
        }

        void *MemoryArena::alloc(size_t bytes) {
            // bytes 需要和字节对齐
            bytes = (bytes + alignBytes - 1) & (~(alignBytes - 1));
            if (_currentOffset + bytes > _allocatedBlockSize) {
                // 需要的字节数目超过了当前分配的 block 长度
                if (_block != nullptr) {
                    // 当前 block 存放到链表中
                    _usedBlocks.push_back(std::make_pair(_allocatedBlockSize, _block));
                    _block = nullptr;
                    _allocatedBlockSize = 0;
                }

                for (auto iter = _availableBlocks.begin(); iter != _availableBlocks.end(); iter++) {
                    // 从备用 block 表中查找是否有可以使用的 block
                    if (iter->first >= bytes) {
                        _allocatedBlockSize = iter->first;
                        _block = iter->second;
                        _availableBlocks.erase(iter);
                        break;
                    }
                }

                if (_block == nullptr) {
                    // 若备用 block 没有可以用于分配的，则重新申请内存
                    size_t allocateBytes = std::max(bytes, _blockSize);
                    _block = allocAlignedMemory<uint8_t>(allocateBytes);
                    _allocatedBlockSize = allocateBytes;
                }
                _currentOffset = 0;
            }
            void *result = _block + _currentOffset;
            _currentOffset += bytes;
            return result;
        }

        void MemoryArena::clean() {
            _availableBlocks.splice(_availableBlocks.begin(), _usedBlocks);
            _currentOffset = 0;
        }
    }
}
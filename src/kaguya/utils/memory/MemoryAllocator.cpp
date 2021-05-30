//
// Created by StormPhoenix on 2021/5/28.
//

#ifndef RENDER_CACHE_LINE_SIZE
// Cache line size
#define RENDER_CACHE_LINE_SIZE 64
#endif

#include <kaguya/utils/memory/MemoryAllocator.h>
#include <kaguya/utils/memory/HostResource.h>

namespace RENDER_NAMESPACE {
    namespace memory {

        MemoryAllocator::MemoryAllocator()
                : _defaultBlockSize(262144), _currentBlock(nullptr),
                  _blockOffset(0), _allocatedBlockSize(0) {
            _resource = new HostResource();
        }

        MemoryAllocator::MemoryAllocator(MemoryResource *resource)
                : _defaultBlockSize(262144), _currentBlock(nullptr),
                _blockOffset(0),_allocatedBlockSize(0),
                  _resource(resource) {}

        void *MemoryAllocator::allocate(size_t bytes, size_t alignBytes) {
            // Aligned
            bytes = (bytes + alignBytes - 1) & (~(alignBytes - 1));
            if (_blockOffset + bytes > _allocatedBlockSize) {
                if (_currentBlock != nullptr) {
                    _usedBlocks.push_back(std::make_pair(_allocatedBlockSize, _currentBlock));
                    _currentBlock = nullptr;
                    _allocatedBlockSize = 0;
                }

                for (auto iter = _availableBlocks.begin(); iter != _availableBlocks.end(); iter++) {
                    // Search for available size block
                    if (iter->first >= bytes) {
                        _allocatedBlockSize = iter->first;
                        _currentBlock = iter->second;
                        _availableBlocks.erase(iter);
                        break;
                    }
                }

                if (_currentBlock == nullptr) {
                    size_t allocatedBytes = std::max(bytes, _defaultBlockSize);
                    _currentBlock = static_cast<uint8_t *>(
                            _resource->allocateAlignedMemory(allocatedBytes, RENDER_CACHE_LINE_SIZE));
                    _allocatedBlockSize = allocatedBytes;
                }
                _blockOffset = 0;
            }
            void *ret = _currentBlock + _blockOffset;
            _blockOffset += bytes;
            return ret;
        }

        void MemoryAllocator::reset() {
            _availableBlocks.splice(_availableBlocks.begin(), _usedBlocks);
            _blockOffset = 0;
        }

        MemoryAllocator::~MemoryAllocator() {
            _resource->freeAlignedMemory(_currentBlock);
            for (auto iter = _usedBlocks.begin(); iter != _usedBlocks.end(); iter++) {
                _resource->freeAlignedMemory(iter->second);
            }

            for (auto iter = _availableBlocks.begin(); iter != _availableBlocks.end(); iter++) {
                _resource->freeAlignedMemory(iter->second);
            }
            delete _resource;
        }
    }
}
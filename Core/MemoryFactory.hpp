#ifndef __CORE__MEMORYBLOCK__FACOTRY__
#define __CORE__MEMORYBLOCK__FACOTRY__
#pragma once
#include "MemoryBlock.hpp"
#include "Singleton.hpp"
#include <map>
#include <forward_list>
#include <stdexcept>

namespace Core
{
    namespace Memory
    {
        template <typename T>
        using BlockList = std::forward_list<MemoryBlockPtr<T>>;

        template <typename T>
        class MemoryFactory : public Singleton<MemoryFactory<T>>
        {
            MAKE_SINGLETON(MemoryFactory);

        private:
            
            ui64 MemoryUsed;
            std::map<ui64, BlockList<T>> freeBlocks;
            std::map<ui64, std::map<ui64, MemoryBlockPtr<T>>> usedBlocks;

            ui64 GetUpperSize(ui64 size)
            {
                return size;
            }

        public:
            bool Recycle = false;
            ui64 MemoryMaxUseable = 8 * 1024 * 1024 * 1024;

            void Sort() {}

            std::shared_ptr<MemoryBlock<T>> GetBlock(ui64 count)
            {
                ui64 size = count * sizeof(T);
                ui64 blocksize = GetUpperSize(size);
                MemoryBlockPtr<T> block = nullptr;

                if (MemoryUsed + blocksize > MemoryMaxUseable)
                {
                    throw std::runtime_error("Out of memory!");
                }

                if (freeBlocks.contains(blocksize))
                {
                    BlockList<T> freeList = freeBlocks[blocksize];
                    if (freeList.empty())
                    {
                        /*std::shared_ptr<MemoryBlock<T>>*/
                        block = std::make_shared<MemoryBlock<T>>(blocksize);
                    }
                    else
                    {
                        block = freeList.front();
                        freeList.pop_front();
                    }
                }

                std::map<ui64, MemoryBlockPtr<T>> usedBlocksMap;
                if (usedBlocks.contains(blocksize))
                {
                    usedBlocksMap = usedBlocks[blocksize];
                }
                usedBlocksMap.emplace(block->id, block);
                MemoryUsed += blocksize;

                return block;
            }

            void ReturnBlock(MemoryBlockPtr<T> block)
            {
            }
        };
    }
}
#endif
#ifndef __CORE__MEMORYBLOCK__FACOTRY__
#define __CORE__MEMORYBLOCK__FACOTRY__
#pragma once
#include "MemoryBlock.hpp"
#include "Singleton.hpp"
#include <map>
#include <list>
#include <stdexcept>

namespace Core
{
    namespace Memory
    {
        class MemoryStatistics;

        template <typename T>
        class MemoryFactory;

        template <typename T>
        using BlockList = std::list<MemoryBlockPtr<T>>;

        template <typename T>
        using BlockMap = std::map<ui64, MemoryBlockPtr<T>>;

        template <typename Tval, typename TSame>
        concept IsFacotry = std::is_same<MemoryFactory<Tval>, TSame>::value;

        template <typename Tval, class Tf>
            requires IsFacotry<Tval, Tf>
        std::string UnmemGetInfo();

        template <typename T>
        class MemoryFactory : public Singleton<MemoryFactory<T>>
        {
            MAKE_SINGLETON(MemoryFactory);
            friend class MemoryStatistics;
            friend std::string UnmemGetInfo<T, MemoryFactory<T>>();

        private:
            ui64 MemoryUsed = 0ull;
            ui64 MemoryFree = 0ull;
            std::map<ui64, BlockList<T>> freeBlocks;
            std::map<ui64, BlockMap<T>> usedBlocks;

            std::string GetInfo()
            {
                ui64 totfreeBlocks = 0;
                ui64 totusedBlocks = 0;

                std::string builder = "Memory allocated: " + MemoryFree + MemoryUsed + "\n";
                builder.append("Memory used: " + MemoryUsed + "\n");
                builder.append("Memory free: " + MemoryFree + "\n");

                builder.append("Free blocks info: \n");
                for (auto &free : freeBlocks)
                {
                    // builder.append("block size: " + free.first + " count: " + free.second.size() + "\n");
                }

                builder.append("Used blocks info: \n");
                for (auto &free : usedBlocks)
                {
                    // builder.append("block size: " + free.first + " count: " + free.second.size() + "\n");
                }

                return builder;
            }

        public:
            MemoryFactory()
            {
                MemoryStatistics::Instance()->AddSubscribe(*this);
            }

            bool Recycle = false;
            ui64 MemoryMaxUseable = 8 * 1024ull * 1024ull * 1024ull;

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
                        block = std::make_shared<MemoryBlock<T>>(blocksize);
                    }
                    else
                    {
                        block = freeList.front();
                        freeList.pop_front();
                        MemoryFree -= block->Size();
                    }
                }
                else
                {
                    block = std::make_shared<MemoryBlock<T>>(blocksize);
                    BlockList<T> list;
                    freeBlocks.emplace(blocksize, list);
                }

                BlockMap<T> usedBlocksMap;
                if (!usedBlocks.contains(blocksize))
                {
                    usedBlocks.emplace(blocksize, usedBlocksMap)
                }
                else
                {
                    usedBlocksMap = usedBlocks[blocksize];
                }
                usedBlocksMap[block->id] = block;
                MemoryUsed += blocksize;

                return block;
            }

            void ReturnBlock(MemoryBlockPtr<T> &block)
            {
                ui64 size = block->size;
                usedBlocks[size].erase(block->id);
                freeBlocks[size].emplace_front(block);
                MemoryUsed -= block->Size();
                MemoryFree += block->Size();
            }
        };

        template <typename Tval, class Tf>
            requires IsFacotry<Tval, Tf>
        std::string UnmemGetInfo()
        {
            return MemoryFactory<Tval>::Instance()->GetInfo();
        }
    }
}
#endif
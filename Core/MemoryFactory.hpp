#ifndef __CORE__MEMORYBLOCK__FACOTRY__
#define __CORE__MEMORYBLOCK__FACOTRY__
#pragma once
#include "MemoryBlock.hpp"
#include "Singleton.hpp"
#include <map>
#include <list>
#include <stdexcept>
#include <sstream>

namespace Core
{
    namespace Memory
    {
        class MemoryStatistics;

        struct FactoryInfo{
            ui64 used = 0;
            ui64 free = 0;
        };

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
        std::string UnmemberedGetInfo();

        template <typename Tval, class Tf>
            requires IsFacotry<Tval, Tf>
        FactoryInfo UnmemberedGetFInfo();

        template <typename T>
        class MemoryFactory : public Singleton<MemoryFactory<T>>
        {
            MAKE_SINGLETON(MemoryFactory);
            friend class MemoryStatistics;
            friend std::string UnmemberedGetInfo<T, MemoryFactory<T>>();
            friend FactoryInfo UnmemberedGetFInfo<T, MemoryFactory<T>>();

        private:
            ui64 MemoryUsed = 0ull;
            ui64 MemoryFree = 0ull;
            std::map<ui64, BlockList<T>> freeBlocks;
            std::map<ui64, BlockMap<T>> usedBlocks;

            std::string GetInfo()
            {
                ui64 totfreeBlocks = 0;
                ui64 totusedBlocks = 0;

                std::stringstream builder;
                builder << "Memory allocated: " << MemoryFree + MemoryUsed << std::endl
                        << "Memory used: " << MemoryUsed << std::endl
                        << "Memory free: " << MemoryFree << std::endl;
                builder << "=========================================" << std::endl
                        << "Blocks Info:" << std::endl;
                builder << "Free blocks info:" << std::endl;
                for (auto &p : freeBlocks)
                {
                    auto size = p.second.size();
                    if (size == 0)
                    {
                        continue;
                    }
                    totfreeBlocks += size;
                    builder << "block size: " << p.first << " count: " << size << std::endl;
                }
                builder << "Total free block count:" << totfreeBlocks << std::endl;

                builder << "Used blocks info:" << std::endl;
                for (auto &p : usedBlocks)
                {
                    auto size = p.second.size();
                    if (size == 0)
                    {
                        continue;
                    }
                    totusedBlocks += size;
                    builder << "block size: " << p.first << " count: " << size << std::endl;
                }
                builder << "Total used block count:" << totusedBlocks << std::endl;

                return builder.str();
            }

            FactoryInfo GetFInfo(){
                FactoryInfo info;
                info.free = MemoryFree;
                info.used = MemoryUsed;
                return info;
            }
        public:
            MemoryFactory()
            {
                MemoryStatistics::Instance()->AddSubscribe(*this);
            }

            bool Recycle = false;
            ui64 MemoryMaxUseable = 8 * 1024ull * 1024ull * 1024ull;

            void Sort() {}
            ui64 GetUpperSize(ui64 s)
            {
                return s;
            }

            MemoryBlockPtr<T> GetBlock(ui64 count)
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
                    BlockList<T> &freeList = freeBlocks[blocksize];
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
                    freeBlocks.emplace(blocksize, BlockList<T>());
                }

                if (!usedBlocks.contains(blocksize))
                {
                    usedBlocks.emplace(blocksize, BlockMap<T>());
                }
                BlockMap<T> usedBlocksMap = usedBlocks[blocksize];

                usedBlocksMap.emplace(block->Id(), block);
                MemoryUsed += blocksize;

                return block;
            }

            void ReturnBlock(MemoryBlockPtr<T> &block)
            {
                ui64 size = block->Size();
                this->usedBlocks[size].erase(block->id);
                this->freeBlocks[size].emplace_front(block);
                MemoryUsed -= size;
                MemoryFree += size;
            }
        };

        template <typename Tval, class Tf>
            requires IsFacotry<Tval, Tf>
        std::string UnmemberedGetInfo()
        {
            return MemoryFactory<Tval>::Instance()->GetInfo();
        }

        template <typename Tval, class Tf>
            requires IsFacotry<Tval, Tf>
        FactoryInfo UnmemberedGetFInfo()
        {
            return MemoryFactory<Tval>::Instance()->GetFInfo();
        }
    }
}
#endif
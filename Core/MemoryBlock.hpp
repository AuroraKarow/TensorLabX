#ifndef __CORE__MEMORY__
#define __CORE__MEMORY__
#include "CoreTypes.hpp"
#include <memory>
#pragma once

namespace Core
{
    namespace Memory
    {
        template <typename T>
        class MemoryFactory;

        template <typename T>
        class MemoryBlock
        {
            friend class MemoryFactory<T>;

        private:
            std::shared_ptr<T[]> block;
            ui64 size;

            static ui64 iid;

        public:
            const ui64 id;

            MemoryBlock(ui64 _size = 0) : id(++iid)
            {
                size = _size;
                block = std::make_shared<T[]>(_size);
            }

            MemoryBlock(std::shared_ptr<T[]> p, ui64 _size) : id(++iid)
            {
                size = _size;
                block = p;
            }

            ~MemoryBlock()
            {
            }

            std::shared_ptr<T[]> &Get()
            {
                return block;
            }

            const T *GetRaw()
            {
                auto p = block.get();
                return reinterpret_cast<T *>(block.get());
            }

            ui64 Id() const
            {
                return id;
            }

            ui64 Size() const
            {
                return size;
            }

            void CopyFrom(const MemoryBlock<T> &source)
            {
                ui64 sizeForCopy = size >= source.Size() ? size : source.Size();
                // std::memcpy(block.get(), source.GetRaw<void>(), sizeForCopy);
            }

            MemoryBlock<T> &Clone()
            {
                MemoryBlock<T> _copy(size);
                // std::memcpy(block.get(), _copy.block.get(), size);
                return _copy;
            }

            MemoryBlock<T> &Same()
            {
                MemoryBlock<T> _copy(size);
                return _copy;
            }

            bool operator==(MemoryBlock<T> &_mb)
            {
                if (this == &_mb)
                {
                    return true;
                }
                return block == _mb.block;
            }

            bool operator!=(MemoryBlock<T> &_mb)
            {
                return !(*this == _mb);
            }

            MemoryBlock<T> operator=(MemoryBlock<T> &_mb)
            {
                return MemoryBlock(_mb->block, _mb.size);
            }
        };

        template <typename T>
        ui64 MemoryBlock<T>::iid = 0;

        template <typename T>
        using MemoryBlockPtr = std::shared_ptr<MemoryBlock<T>>;
    } // namespace Memory

} // namespace Core
#endif
#ifndef __CORE__MEMORY__
#define __CORE__MEMORY__
#include "CoreTypes.hpp"
#include <memory>

namespace Core
{
    namespace Memory
    {
        template <typename T>
        class MemoryBlock
        {
        private:
            std::shared_ptr<T[]> block;
            ui64 size;

        public:
            MemoryBlock(ui64 size);
            ~MemoryBlock();

            std::shared_ptr<T[]> Get();
            *T GetRaw();
            ui64 Size();
        };

        template <typename T>
        MemoryBlock<T>::MemoryBlock(ui64 _size)
        {
            size = _size;
            block = std::make_shared<T[]>(_size);
        }

        template <typename T>
        MemoryBlock<T>::~MemoryBlock()
        {
        }

        template <typename T>
        std::shared_ptr<T[]> MemoryBlock<T>::Get()
        {
            return block;
        }

        template <typename T>
        T* MemoryBlock<T>::GetRaw()
        {
            return block.get();
        }

        template <typename T>
        ui64 MemoryBlock<T>::Size()
        {
            return size;
        }

    } // namespace Memory

} // namespace Core
#endif
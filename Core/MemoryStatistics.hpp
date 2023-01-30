#ifndef __CORE__MEMORYSTATISTICS__
#define __CORE__MEMORYSTATISTICS__
#pragma once

#include "CoreTypes.hpp"
#include "MemoryFactory.hpp"
#include "Singleton.hpp"
#include <typeinfo>
#include <map>
#include <functional>

namespace Core
{
    namespace Memory
    {
        class MemoryStatistics : public Singleton<MemoryStatistics>
        {
            MAKE_SINGLETON(MemoryStatistics);

        private:
            std::map<size_t, std::function<std::string()>> infoInvoker;
            std::map<size_t, std::string> typeNames;

        public:
            MemoryStatistics(/* args */)
            {
            }

            template <typename Tval>
            void AddSubscribe(MemoryFactory<Tval> &factory)
            {
                const std::type_info &t = typeid(Tval);
                size_t hash = t.hash_code();
                typeNames.emplace(hash, t.name());
                auto ii = UnmemGetInfo<Tval, MemoryFactory<Tval>>;
                infoInvoker.emplace(hash, ii);
            }

            std::string GetInfo()
            {
                std::string builder;
                ui64 totUsed = 0;
                ui64 totFree = 0;
                for (auto &t : this->typeNames)
                {
                    size_t hash = t.first;
                    builder.append("MemoryFactory<" + t.second + ">");
                    builder.append("\n" + infoInvoker[hash]() + "\n");
                }

                return builder;
            }
        };

    } // Memory
} // CORE

#endif
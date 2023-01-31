#ifndef __CORE__MEMORYSTATISTICS__
#define __CORE__MEMORYSTATISTICS__
#pragma once

#include "CoreTypes.hpp"
#include "MemoryFactory.hpp"
#include "Singleton.hpp"
#include <typeinfo>
#include <map>
#include <functional>
#include <sstream>

namespace Core
{
    namespace Memory
    {
        class MemoryStatistics : public Singleton<MemoryStatistics>
        {
            MAKE_SINGLETON(MemoryStatistics);

        private:
            std::map<size_t, std::function<std::string()>> strInfoInvoker;
            std::map<size_t, std::function<FactoryInfo()>> infoInvoker;
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
                auto strii = UnmemberedGetInfo<Tval, MemoryFactory<Tval>>;
                auto ii = UnmemberedGetFInfo<Tval, MemoryFactory<Tval>>;
                strInfoInvoker.emplace(hash, strii);
                infoInvoker.emplace(hash, ii);
            }

            std::string GetInfo()
            {
                std::ostringstream builder;
                ui64 totUsed = 0;
                ui64 totFree = 0;
                for (auto &t : this->typeNames)
                {
                    size_t hash = t.first;
                    builder << "MemoryFactory<" << t.second << ">"
                            << std::endl
                            << strInfoInvoker[hash]() << std::endl;

                    auto info = infoInvoker[hash]();
                    totUsed += info.used;
                    totFree += info.free;
                }
                builder << "Summary:" << std::endl
                        << "Total Used Bytes: " << totUsed << std::endl
                        << "Total Free Bytes: " << totFree << std::endl
                        << "Total Bytes: " << totUsed + totFree << std::endl;
                return builder.str();
            }
        };
    } // Memory
} // CORE

#endif
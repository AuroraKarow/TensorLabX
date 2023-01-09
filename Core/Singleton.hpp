#ifndef __CORE__SINGLETON__
#define __CORE__SINGLETON__

#include <memory>

template <class T>
class Singleton
{
protected:
    static std::shared_ptr<T> instance;

public:
    static std::shared_ptr<T>& Instance();
};

template <class T>
std::shared_ptr<T> Singleton<T>::instance{new T()};

template <class T>
std::shared_ptr<T>& Singleton<T>::Instance()
{
    return instance;
}

#define MAKE_SINGLETON(Type)      \
    friend class Singleton<Type>; \
//protected:                        \
//    Type() = default;             \
//                                  \
public:

#endif //__CORE__SINGLETON__
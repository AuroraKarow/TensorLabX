#ifndef __CORE__SINGLETON__
#define __CORE__SINGLETON__

#include <memory>

template <class T>
class Singleton
{
protected:
    static std::unique_ptr<T> instance;

public:
    static T *Instance();
};

template <class T>
std::unique_ptr<T> Singleton<T>::instance{new T()};

template <class T>
T *Singleton<T>::Instance()
{
    return instance.get();
}

#define MAKE_SINGLETON(Type)      \
    friend class Singleton<Type>; \
//protected:                        \
//    Type() = default;             \
//                                  \
public:

#endif //__CORE__SINGLETON__
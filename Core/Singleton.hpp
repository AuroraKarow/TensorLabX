#ifndef __CORE__SINGLETON__
#define __CORE__SINGLETON__
#pragma once
#include <memory>

template <class T>
class Singleton
{
public:
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
    friend class Singleton<Type>; 

#endif //__CORE__SINGLETON__
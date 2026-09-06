#pragma once
#include <memory>
#include "ObjectPool.h"

//forward
//완벽한 전달
//lvalue 면 lvalue, rvalue 면 ravlue로

#define DESTROY(ptr) if( ptr ){ ptr->Destroy(); Delete(ptr); }


//#define _USE_MEMORY_POOL
 
//================USE MEMORY POOL MACRO==============
#ifdef _USE_MEMORY_POOL
template<typename T>
using Ptr = T*; //메모리풀을 사용할때는 원시포인터

template<typename T>
using Weak = T*; //메모리풀을 사용할때는 약참조 포인터도 원시포인터

template<typename T, typename ...Args>
Ptr<T> New(Args&&... args)
{
    return new T(std::forward<Args>(args)...);
}

template<typename T>
Weak<T> MakeWeak(Ptr<T>& ptr)
{
    return ptr;
}

template<typename T>
void Delete(Ptr<T>& ptr)
{
    delete ptr;
    ptr = nullptr;
}

template<typename Src, typename Dest>
Ptr<Dest> Cast(Ptr<Src> src)
{
    return dynamic_cast<Ptr<Dest>>(src);
}

template<typename T>
Ptr<T> Lock(Weak<T> ptr)
{
    return ptr;
}
//================USE OBJECT POOL MACRO==============
#elif _USE_OBJECT_POOL
template<typename T>
using Ptr = T*; //오브젝트 풀 을 사용할때는 원시포인터

template<typename T>
using Weak = T*; //오브젝트 풀 을 사용할때는 약참조 포인터도 원시포인터

template<typename T, typename ...Args>
Ptr<T> New(Args&&... args)
{
    return ObjectPool::Instance().Rent<T>();
}

template<typename T>
Weak<T> MakeWeak(Ptr<T>& ptr)
{
    return ptr;
}

template<typename T>
void Delete(Ptr<T>& ptr)
{
    ObjectPool::Instance().Return(ptr);
    ptr = nullptr;
}

template<typename Src, typename Dest>
Ptr<Dest> Cast(Ptr<Src> src)
{
    return dynamic_cast<Ptr<Dest>>(src);
}

template<typename T>
Ptr<T> Lock(Weak<T> ptr)
{
    return ptr;
}

//================OTHER MACRO==============
#else
template<typename T>
using Ptr = std::shared_ptr<T>; //오브젝트 풀 을 사용할때는 원시포인터

template<typename T>
using Weak = std::weak_ptr<T>; //오브젝트 풀 을 사용할때는 약참조 포인터도 원시포인터

template<typename T, typename ...Args>
Ptr<T> New(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
Weak<T> MakeWeak(Ptr<T>& ptr)
{
    return ptr;
}

template<typename T>
void Delete(Ptr<T>& ptr)
{
    ptr = nullptr;
}

template<typename Src, typename Dest>
Ptr<Dest> Cast(Ptr<Src> src)
{
    return std::dynamic_pointer_cast<Dest>(src);
}

template<typename T>
Ptr<T> Lock(Weak<T> ptr)
{
    return ptr.lock();
}

#endif

#define DECLARE_SINGLETON(className)\
  friend class Singleton<className>;\
protected:                                              \
    className() = default;                              \
    virtual ~className() = default;                     \
    className(const className&) = delete;               \
    className(className&&) = delete;                    \
    className& operator = (const className&) = delete;  \
    className& operator = (className&&) = delete;       \
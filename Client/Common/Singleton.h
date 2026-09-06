#pragma once
#include "Core/Defines.h"

template<typename T>
class Singleton
{
protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

public:
    static T& Instance()
    {
        static T Inst;
        return Inst;
    }

public:
    virtual void Destroy() = 0;
};
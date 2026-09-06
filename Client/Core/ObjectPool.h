#pragma once
#ifdef _USE_OBJECT_POOL
#include "Common/Singleton.h"
#include "Common/Info.h"

#include <typeindex>
#include <map>
#include <queue>

class ObjectPool : public Singleton<ObjectPool>
{
    friend Singleton<ObjectPool>;
protected:
    ObjectPool() = default;
    ~ObjectPool() = default;
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool(ObjectPool&&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
    ObjectPool& operator=(ObjectPool&&) = delete;

    //오브젝트 풀
    std::map<std::type_index, std::queue<class Object*>> _Pools;

private:
    template<typename T>
    void PreAlloc(uint32 count)
    {
        for (uint32 i = 0; i < count; ++i)
        {
            T* obj = new T();
            _Pools[std::type_index(typeid(T))].push(obj);
        }
    }

public:
    bool Init();
    virtual void Destroy() override;
    void Return(class Object* obj);

    template<typename T>
    T* Rent()
    {
        auto pool = _Pools.find(std::type_index(typeid(T)));
        if (pool == _Pools.end())
            return nullptr;

        T* retVal = nullptr;
        if (pool->second.empty())
        {
            retVal = new T();

            return retVal;
        }
        else
        {
            retVal = dynamic_cast<T*>(pool->second.front());
            if (nullptr == retVal)
                return nullptr;

            pool->second.pop();
        }

        return retVal;
    }
};
#endif




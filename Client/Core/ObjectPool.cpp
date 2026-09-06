#include "pch.h"
#include "ObjectPool.h"

#ifdef _USE_OBJECT_POOL

bool ObjectPool::Init()
{
    //오브젝트 풀에 오브젝트를 미리 생성한다.

    return true;
}

void ObjectPool::Destroy()
{
    //오브젝트 풀 목록을 순회한다.
    auto it = _Pools.begin();
    auto itEnd = _Pools.end();
    for (; it != itEnd; ++it)
    {
        //오브젝트 풀 - 오브젝트 큐가 비어있을때까지 반복한다.
        while (!it->second.empty())
        {
            //오브젝트 풀 - 오브젝트 큐를 하나하나 동적할당을 해제 한다.
            Object* obj = it->second.front();
            if (nullptr != obj)
            {
                delete obj;
                obj = nullptr;
            }

            it->second.pop();
        }
    }
}

void ObjectPool::Return(Object* obj)
{
    if (nullptr == obj)
        return;

    if (false == obj->_IsPooled)
    {
        delete obj;
        return;
    }

    obj->SetActive(false);

    _Pools[std::type_index(typeid(*obj))].push(obj);

}
#endif
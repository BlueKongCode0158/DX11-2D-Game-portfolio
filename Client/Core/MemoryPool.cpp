#include "pch.h"
#include "MemoryPool.h"

#ifdef _USE_MEMORY_POOL
#include <iostream>

bool MemoryPool::Init()
{
    int pow = _DefaultPow;
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 100));   //16byte * 100
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 100));   //32byte * 100
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 50));    //64byte * 50
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 50));    //128byte * 50
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 10));    //256byte * 10
    _Pools.push_back(new SegmentPool(static_cast<int32>(std::pow(2, pow++)), 10));    //512byte * 10

    return true;
}

void MemoryPool::Destroy()
{
    for (auto& it : _Rentals)
    {
        int index = IndexFromSize(it.second._Size);
        SegmentPool* pool = Find(index);
        pool->Return(it.second);
    }
    _Rentals.clear();

    for (auto* pool : _Pools)
        delete pool;
}

FSegment MemoryPool::Rent(int32 size)
{
    int32 index = IndexFromSize(size);
    SegmentPool* pool = Find(index);
    if (!pool)
    {
        FSegment seg;
        seg._Data = new byte[size];
        seg._IsPooled = false;
        seg._Size = size;

        return seg;
    }

    FSegment seg = pool->Rent();
    _Rentals[seg._Data] = seg;
    return seg;
}

void MemoryPool::Return(FSegment& seg)
{
    if (!seg._IsPooled)
    {
        delete[] seg._Data;
        return;
    }

    int32 index = IndexFromSize(seg._Size);
    SegmentPool* pool = Find(index);
    pool->Return(seg);
}

void MemoryPool::Return(void* memory)
{
    if (_Rentals.end() == _Rentals.find(memory))
    {
        delete memory;
        return;
    }

    int32 index = IndexFromSize(_Rentals[memory]._Size);
    SegmentPool* pool = Find(index);
    pool->Return(_Rentals[memory]);
    _Rentals.erase(memory);
}

int32 MemoryPool::IndexFromSize(int32 size)
{
    int32 log = static_cast<int32>(std::ceil(std::log2(size)));
    int32 idx = log - _DefaultPow;
    return idx;
}

SegmentPool* MemoryPool::Find(int32 idx)
{
    if (idx < 0 || idx >= _Pools.size())
        return nullptr;

    return _Pools[idx];
}
#endif
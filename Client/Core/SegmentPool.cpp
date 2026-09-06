#include "pch.h"
#include "SegmentPool.h"

SegmentPool::SegmentPool(int32 buffSize, int32 bucketSize) : _BuffSize(buffSize), _BucketSize(bucketSize)
{
    Create();
}

SegmentPool::~SegmentPool()
{
    for (auto* chunk : _Chunks)
        delete[] chunk;
}

void SegmentPool::Create()
{
    //_BuffSize * _BucketSize 만큼의 크기를 동적할당 해놓는 것
    byte* chunk = new byte[_BuffSize * _BucketSize]();
    _Chunks.push_back(chunk);


    //할당한 청크를 정해진 버킷 갯수와, 버퍼사이즈 로
    //쪼개서 큐에 넣는다.
    for (size_t i = 0; i < _BucketSize; ++i)
    {
        FSegment seg;
        seg._Data = chunk + (i * _BuffSize);
        seg._Size = _BuffSize;
        seg._IsPooled = true;

        _Buckets.push(seg);
    }
}

FSegment SegmentPool::Rent()
{
    //버킷이 있다면
    if (!_Buckets.empty())
    {
        //버킷큐에서 버킷을 하나 빌려온다.
        FSegment seg = _Buckets.front();
        _Buckets.pop();

        return seg;
    }

    //버킷이 충분하지 않은거
    //1. Create
    //2. 새로 필요한 만큼만 할당한다 <<

    //버킷이 충분하지 않으므로 직접 동적할당하여
    //제공한다.
    FSegment seg;
    seg._Data = new byte[_BuffSize]();
    seg._IsPooled = false;
    seg._Size = _BuffSize;

    return seg;
}

void SegmentPool::Return(FSegment& seg)
{
    //seg가 풀에서 빌려온 메모리가 아니므로
    //직접 해제해준다.
    if (!seg._IsPooled)
    {
        delete[] seg._Data;
        return;
    }

    //풀에서 빌려온 메모리를 다시 풀로 되돌려준다.
    _Buckets.push(seg);
}


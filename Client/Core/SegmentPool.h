#pragma once
#include <vector>
#include <queue>

//unsigned char => 1byte
using byte = unsigned char;

struct FSegment
{
    byte* _Data = nullptr;  //실제 메모리 주소
    uint32 _Size = 0; //이 세그먼트의 크기
    bool _IsPooled = false; //풀에서 할당한것인지 여부
};

class SegmentPool
{
    friend class MemoryPool;

private:
    SegmentPool(int32 buffSize, int32 bucketSize);
    ~SegmentPool();
    SegmentPool(const SegmentPool&) = delete;
    SegmentPool(SegmentPool&&) = delete;
    SegmentPool& operator = (const SegmentPool&) = delete;
    SegmentPool& operator = (SegmentPool&&) = delete;


private:
    int32 _BuffSize = 0; //버킷 하나의 크기(바이트)
    int32 _BucketSize = 0; //버킷의 개수

    //실제 할당한 메모리
    std::vector<byte*> _Chunks;

    //대여 가능한 버킷 목록
    //_Chunks에 할당한 메모리를 _BuffSize 만큼 쪼개서
    //_BucketSize 개수만큼 _Buckets 여기에 넣어놓는다.
    std::queue<FSegment> _Buckets;

private:
    //새 청크를 할당하고, 청크를 버킷으로 쪼개서 큐에 추가
    void Create();

public:
    //버킷 하나를 대여
    FSegment Rent();

    //사용한 버킷을 다시 반납
    void Return(FSegment& seg);
};


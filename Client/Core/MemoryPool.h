#pragma once
//메모리 풀
//사용할 메모리를 먼저 많이 할당해놓고, 거기서 필요할때마다 필요한 크기만큼의
//메모리를 가져온다.

//메모리 풀 , 오브젝트 풀
//메모리 풀 --> 메모리를 재사용(객체X)
//자주 할당/해제 되는 메모리 블록(바이트 덩어리)을 미리 크게 확보
//필요할 때 가져다 쓰고, 다 쓰면 반납
//메모리(바이트 덩어리)를 다룬다. (객체X)

//오브젝트 풀 --> 오브젝트를 재사용(객체 O)
//생성 비용이 큰 객체를 미리 여러개 생성
//필요할때 객체를 꺼내다 쓰고, 다 쓰면 반납

//client 에서는 굳이굳이 메모리풀..? 잘 안써요..
//왜냐면.. 쓸 이유가 그렇게 크지 않아요.
//why? 생성 해제하는 객체들이 크기가 빈번하게 변하지 않아요(ex. 몬스터, 총알, 아이템 등등..)
//동적으로 생성하는게 거기서 거기
//오히려 오브젝트 풀이 조금 더 효과적일수도 있다.

//언리얼이 내부적으로 메모리풀 + GC
#ifdef _USE_MEMORY_POOL
#include "SegmentPool.h"
#include "Common/Singleton.h"
#include <vector>
#include <unordered_map>

class MemoryPool : public Singleton<MemoryPool>
{
    friend Singleton<MemoryPool>;

protected:
    MemoryPool() = default;
    ~MemoryPool() = default;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;


private:
    //메모리 풀이 SegmentPool을 여러개 들고 있다.
    std::vector<class SegmentPool*> _Pools;
    std::unordered_map<void* , FSegment> _Rentals;
    const int32 _DefaultPow = 4;

public:
    bool Init();
    virtual void Destroy() override;
    
    //요청 크기에 맞는 메모리를 빌려준다.
    FSegment Rent(int32 size);

    //대여한 메모리를 반납
    void Return(FSegment& seg);

    //대여한 메모리를 반납
    void Return(void* memory);

private:
    int IndexFromSize(int32 size);
    SegmentPool* Find(int32 idx);
};

#endif
#pragma once
#include "Common/Singleton.h"
#include "Core/Timer.h"

#include <chrono> //C++에서 사용하는 시간 관련 헤더

class TimeManager : public Singleton<TimeManager>
{
    DECLARE_SINGLETON(TimeManager);
private:
    //std::chrono::high_resolution_clock::time_point
    //특정 시간을 나타내는 타임스탬프 -->
    //steady clock : 부팅시간 시계
    //sytem clock : 시계 --> timestamp --> epochTime;
    //1970.01.01 --> 
    std::chrono::high_resolution_clock::time_point _prevTime;
    float _deltaTime = 0.f;
    std::map<int32, Ptr<Timer>> _timers;
    std::vector<int32> _removeTimers;
    int32 _timerID = -1;
public:
    bool Init();
    virtual void Destroy() override;
    float Tick();
    float GetDeltaTime() const { return _deltaTime; }
    float GetFPS();

    void RemoveTimer(int32 id);
public:
    template<typename T>
    int32 SetTimer(float expireTime, bool repeat, T&& func)
    {
        int32 id = _timerID;
        Ptr<Timer> timer = New<Timer>();
        timer->SetTimer(expireTime, repeat, func);
        _timers[id] = timer;

        return id;
    }

    template<typename T>
    int32 SetTimer(float expireTime, bool repeat, T* obj, void(T::* func)())
    {
        int32 id = _timerID;
        Ptr<Timer> timer = New<Timer>();
        timer->SetTimer(expireTime, repeat, obj, func);
        _timers[id] = timer;

        return id;
    }
};


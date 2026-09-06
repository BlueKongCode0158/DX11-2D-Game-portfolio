#include "pch.h"
#include "TimeManager.h"

bool TimeManager::Init()
{
    _prevTime = std::chrono::high_resolution_clock::now();

    return true;
}

void TimeManager::Destroy()
{
    for (auto& it : _timers)
    {
        DESTROY(it.second);
    }

    _timers.clear();
    _removeTimers.clear();
}

float TimeManager::Tick()
{
    auto curTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> frameTime = curTime - _prevTime;
    _deltaTime = frameTime.count();
    _prevTime = curTime;

    constexpr float MAX_DELTA_TIME = 0.1f; // 최소 10 FPS 보장
    if (_deltaTime > MAX_DELTA_TIME)
    {
        _deltaTime = MAX_DELTA_TIME;
    }
    for (int32 id : _removeTimers)
    {
        auto it = _timers.find(id);
        if (_timers.end() == it)
        {
            continue;
        }

        DESTROY(it->second);
        _timers.erase(id);
    }
    _removeTimers.clear();

    for (auto& it : _timers)
    {
        if (nullptr == it.second)
        {
            RemoveTimer(it.first);
            continue;
        }

        it.second->Tick(_deltaTime);
        if (!it.second->IsRun())
        {
            RemoveTimer(it.first);
        }
    }

    return _deltaTime;
}

float TimeManager::GetFPS()
{
    if (_deltaTime <= 0.f)
        return 0.f;

    return 1.f / _deltaTime;
}

void TimeManager::RemoveTimer(int32 id)
{
    _removeTimers.push_back(id);
}

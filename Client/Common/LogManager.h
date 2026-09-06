#pragma once
#include "Singleton.h"
#include "Core/Logger.h"
#include <fstream> //C++에서 사용하는 파일 스트림
#include <thread>
#include <mutex>

// thread를 많이 쓰면 디버깅하기 힘들어진다.

class LogManager : public Singleton<LogManager>
{
    DECLARE_SINGLETON(LogManager);
private:
    Logger _logger;
    std::ofstream _file;
    uint32 _lineCount = 0;
    uint32 _fileIndex = 0;
    bool _IsRun = false;

    // thread : 프로세스 내에서의 실행흐름   -> 코어개수의 2개정도가 적절하다.
    // 2개 이상의 쓰레드를 사용하는 걸 멀티쓰레드라고 한다.
    // 쓰레드가 너무 많이 있어도 성능이 계속 좋아지는 건 아니다.
    // 쓰레드 간의 context switch(문맥교환) 비용이 들기 때문이다.
    // 각각의 쓰레드들은 힙영역을 공유한다.
    std::thread _logThread;

    // race condition -- 경합 상태, 경쟁 상태.
    // 여러개 쓰레드가 있을 때
    // 쓰레드들은 동시에 접근하는 것.
    // 그래서 lock을 건다.

    // 한번에 한 쓰레드만 접근할 수 있게 하는 것
    std::mutex _m;
   
public:
    void Init();
    virtual void Destroy() override;
    void FlushLog();
    void ProcessLog();

private:
    void OpenNewFile();

public:
    template<typename T, typename ...Args>
    void Debug(const T& data, const Args& ...args)
    {
        std::lock_guard lock(_m); // Enter lock

    // 임계영역 -> lock이 걸리고 lock이 풀리는 순간까지
        _logger.Logging(eLogLevel::Debug, data, args...);
    // lock이 안 풀리는 상황을 dead lock이라고 한다.
    }

    template<typename T, typename ...Args>
    void Info(const T& data, const Args& ...args)
    {
        std::lock_guard lock(_m);
        _logger.Logging(eLogLevel::Info, data, args...);
    }

    template<typename T, typename ...Args>
    void Warning(const T& data, const Args& ...args)
    {
        std::lock_guard lock(_m);
        _logger.Logging(eLogLevel::Warning, data, args...);
    }

    template<typename T, typename ...Args>
    void Error(const T& data, const Args& ...args)
    {
        std::lock_guard lock(_m);
        _logger.Logging(eLogLevel::Error, data, args...);
    }

    template<typename T, typename ...Args>
    void Fatal(const T& data, const Args& ...args)
    {
        std::lock_guard lock(_m);
        _logger.Logging(eLogLevel::Fatal, data, args...);
    }
};

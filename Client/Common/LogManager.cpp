#include "pch.h"
#include "LogManager.h"
#include "Core/DirectoryManager.h"

#define MAX_LINES 1000

void LogManager::Init()
{
    //log level 지정
    _logger.Init(eLogLevel::Debug);
    _IsRun = true;
    _logThread = std::thread(&LogManager::ProcessLog, this);
    OpenNewFile();
}

void LogManager::Destroy()
{
    _IsRun = false;

    if (_logThread.joinable())
    {
        _logThread.join();
    }
    FlushLog();
}

void LogManager::FlushLog()
{
    std::lock_guard lock(_m);
    //todo : print screen , thread func...
    while (!_logger.Logs.empty())
    {
        //쌓인 로그들을 파일에 저장하거나,
        //게임 화면에 출력한다.
        std::string msg = _logger.Logs.front();
        _logger.Logs.pop();

        if (!_file.is_open())
            continue;

        msg += "\n";
        OutputDebugStringA(msg.c_str());

        _file << msg << "\n";
        _lineCount++;

        if (_lineCount > MAX_LINES)
        {
            _lineCount = 0;
            _fileIndex++;
            OpenNewFile();
        }
    }
}

void LogManager::ProcessLog()
{
    while (_IsRun)
    {
        FlushLog();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void LogManager::OpenNewFile()
{
    auto logPath = DirectoryManager::Instance().GetCachePath("Logs");
    if (!logPath)
        return;

    //logPath.value();
    //*logPath
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    char fileName[128] = {};
    sprintf_s(fileName, "%d_%d_%d_%d_%d_%d.txt",
        sysTime.wYear, sysTime.wMonth, sysTime.wDay,
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    std::filesystem::path filePath = *logPath / fileName;

    if (_file.is_open())
        _file.close();

    _file.open(filePath, std::ios::out | std::ios::app);
}

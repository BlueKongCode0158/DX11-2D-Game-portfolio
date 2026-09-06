// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
//헤더파일 에서 이전 경로로
// ../
#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "Core/ObjectPool.h"
#include "Core/MemoryPool.h"
#include "Core/Defines.h"
#include "Core/GameEngine.h"
#include "Editor/EditEngine.h"

#include <fstream>
#include <filesystem>
#include <windows.h>

#define MAX_LOADSTRING 100

bool g_isRun = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

  /*  WriteStartupDebugLog();*/

    GameEngine::Instance().Init(hInstance, L"Knight");

#ifdef _EDITOR
    EditEngine::Instance().Init();
    EditEngine::Instance().Run();
#else
    GameEngine::Instance().Run();
#endif // _EDITOR

    GameEngine::Instance().Destroy();
#ifdef _EDITOR
    EditEngine::Instance().Destroy();
#endif // _EDITOR


    return TRUE;
}


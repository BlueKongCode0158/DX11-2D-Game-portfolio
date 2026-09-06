#include "pch.h"
#include "GameEngine.h"
#include "DirectoryManager.h"
#include "TimeManager.h"
#include "MemoryPool.h"
#include "ObjectPool.h"
#include "Device.h"
#include "Input/Input.h"
#include "Input/InputSystem.h"
#include "Common/LogManager.h"
#include "World/World.h"
#include "Object/Asset/Mesh/Mesh.h"
#include "Object/Asset/Mesh/MeshManager.h"
#include "Object/Asset/AssetManager.h"
#include "Shader/ShaderManager.h"
#include "../Render/RenderManager.h"
#include "Collision/CollisionProfileManager.h"
#include "Defines.h"

// test

void GameEngine::Destroy()
{
    _world->Destroy();

    AssetManager::Instance().Destroy();
    ShaderManager::Instance().Destroy();
#ifdef _USE_MEMORY_POOL
    MemoryPool::Instance().Destroy();
#elif defined(_USE_OBJECT_POOL)
    ObjectPool::Instance().Destroy();
#endif
    _input->Destroy();
    Device::Instance().Destroy();
    LogManager::Instance().Destroy();
    DirectoryManager::Instance().Destroy();
}

bool GameEngine::Init(HINSTANCE inst, const wchar_t* name)
{
    _hInst = inst;

    lstrcpy(_className, name);
    lstrcpy(_titleName, name);

    RegisterWindowClass();

    //윈도우 창 생성
    if (!Create())
        return false;

    _hdc = GetDC(_hWnd);

    FVector3D intScale = FVector3D(1.0f, 1.0f, 1.0f);
    FVector3D intPos = FVector3D(1.f, 1.f, 10.f);
    g_isRun = InitManager();

    _world = New<World>();
    _world->Init("World");
    return true;
}

int32 GameEngine::Run()
{
    MSG msg = {};

    TimeManager::Instance().Init();
    while (g_isRun)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Logic();
        }
    }


    return (int)msg.wParam;
}

bool GameEngine::Create()
{
    _hWnd = CreateWindowW(_className, _titleName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, _hInst, nullptr);

    if (!_hWnd)
    {
        return false;
    }

    RECT windowRect = { 0 ,0, _width, _height };

    //윈도우 클라이언트 영역을 1280, 720에 맞춘다.
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);
    SetWindowPos(_hWnd, HWND_TOPMOST, 0, 0, windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER);

    //생성한 윈도우(창)을 화면에 보이도록 설정
    ShowWindow(_hWnd, SW_SHOW);
    UpdateWindow(_hWnd);

    return true;
}


void GameEngine::RegisterWindowClass()
{
    //생성시킬 윈도우(창)의 옵션 설정
    WNDCLASSEXW wcex;
    //윈도우 클래스 구조체 크기, 반드시 지정이 되어야 합니다.
    wcex.cbSize = sizeof(WNDCLASSEX);
    //화면에 출력 가능한 영역을 클라이언트 영역
    //클라이언트 영역이 변경 될 경우 다시 그려주도록 한다.
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    //
    wcex.lpfnWndProc = WndProc; //함수포인터로 윈도우 메시지 처리 함수
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;

    //운영체제가 부여해준 ID
    wcex.hInstance = _hInst;
    //Icon
    wcex.hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_CLIENT)); //ICON
    //cursor (마우스 포인터)
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //클라이언트 영역 색
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //윈도우 메뉴
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = _className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);
}

void GameEngine::Logic()
{
    float deltaTime = TimeManager::Instance().Tick();
    Tick(deltaTime);
    Collision(deltaTime);
    Render(deltaTime);
}

void GameEngine::Tick(float deltaTime)
{
    _input->Tick(deltaTime);
    InputSystem::Instance().Tick(deltaTime);
    SOUND_MANAGER->Tick();
    _world->Tick(deltaTime);
    RenderManager::Instance().Tick(deltaTime);

    //static int32 fpsCounter = 0;
    //if (++fpsCounter >= 60)
    //{
    //    fpsCounter = 0;
    //    float fps = TimeManager::Instance().GetFPS();
    //    LogManager::Instance().Debug("FPS : ", fps);
    //}
}


void GameEngine::Collision(float deltaTime)
{
    _world->Collision(deltaTime);
}

void GameEngine::Render(float deltaTime)
{
    Device::Instance().ClearSceneTarget(_clearColor);
    Device::Instance().ClearDepthStencil(1.f, 0);

    //	// 1) SceneTartget 
    Device::Instance().SetSceneTarget();

    _world->Render(deltaTime);
    RenderManager::Instance().Render(deltaTime);
    _world->RenderUI(deltaTime);

    Device::Instance().PresentScene();
}

bool GameEngine::InitManager()
{
    DirectoryManager::Instance().Init();
    LogManager::Instance().Init();
#ifdef _USE_MEMORY_POOL
    if (false == MemoryPool::Instance().Init())
    {
        LogManager::Instance().Fatal("MemoryPool Init Error!");
        return false;
    }
#elif defined(_USE_OBJECT_POOL)
    if (false == ObjectPool::Instance().Init())
    {
        LogManager::Instance().Fatal("ObjectPool Init Error!");
        return false;
    }
#endif

    if (false == TimeManager::Instance().Init())
    {
        LogManager::Instance().Fatal("TimeManager Init Error!");
        return false;
    }

    if (false == Device::Instance().Init(_hWnd, _width, _height, true))
    {
        LogManager::Instance().Fatal("Device Init Error!");
        return false;
    }

    _input = New<Input>();
    _input->Init();

    if (false == ShaderManager::Instance().Init())
    {
        LogManager::Instance().Fatal("ShaderManager Init Error!");
        return false;
    }

    if (false == AssetManager::Instance().Init())
    {
        LogManager::Instance().Fatal("AssetManager Init Error!");
        return false;
    }

    if (false == InputSystem::Instance().Init(_input))
    {
        LogManager::Instance().Fatal("InputSystem Init Error!");
        return false;
    }

    if (false == CollisionProfileManager::Instance().Init())
    {
        LogManager::Instance().Fatal("CollisionProfileManager Init Error!");
        return false;
    }

    if (false == RenderManager::Instance().Init())
    {
        LogManager::Instance().Fatal("RenderManager Init Error!");
        return false;
    }
    return true;
}

LRESULT GameEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        g_isRun = false;
        PostQuitMessage(0);
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

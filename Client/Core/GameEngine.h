#pragma once
#include "Client.h"
#include "framework.h"
#include "Common/Singleton.h"

#include "../Editor/EditEngine.h"

class GameEngine : public Singleton<GameEngine>
{
    DECLARE_SINGLETON(GameEngine);
    friend EditEngine;
private:
    HINSTANCE _hInst = nullptr; //운영체제가 발급해주는 ID
    HWND _hWnd = nullptr;       //창마다 생기는 핸들, 여러개 생길 수 있음
    HDC  _hdc = nullptr;        //그리기 위한 핸들
    TCHAR _className[256] = {}; //클래스 이름
    TCHAR _titleName[256] = {}; //윈도우 창 타이틀 이름
    float _clearColor[4] = { 0.f, 0.f, 0.f, 1.f }; //바탕을 어떤색으로 초기화 시킬 지
    Ptr<class World> _world;
    Ptr<class Input> _input;
private:
    int32 _width = 1280;
    int32 _height = 720;
public:
    virtual void Destroy() override;
    //초기화 함수
    bool Init(HINSTANCE inst, const wchar_t* name);
    //실행 함수
    int32 Run();

    HDC GetHDC() const { return _hdc; }
    HINSTANCE GetHINSTANCE() const { return _hInst; }
    HWND GetHWND() const { return _hWnd; }
    Ptr<class World> GetWorld() const { return _world; }
public:
    void SetWidth(int32 width) { _width = width; }
    void SetHeight(int32 height) { _height = height; }
private:
    bool Create(); //윈도우 창 생성 함수
    void RegisterWindowClass(); //윈도우 클래스 생성 함수
    void Logic(); //로직 실행 함수
    void Tick(float deltaTime); //매 프레임 들어오는 함수
    void Collision(float deltaTime);
    void Render(float deltaTime);
    bool InitManager();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#define GAME_ENGINE GameEngine::Instance()
#define GET_WORLD GameEngine::Instance().GetWorld()
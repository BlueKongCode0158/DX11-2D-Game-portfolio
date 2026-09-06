#pragma once
#include "Object/Object.h"
#define DIRECTINPUT_VERSION 0x0800
#define DIK_MOUSELBUTTON 0xfc
#define DIK_MOUSERBUTTON 0xfd
#define DIK_MOUSEWHEEL	 0xfe
#define KEY_PUSH 0x80

#include <dinput.h>	// dx 제공 input
#pragma comment(lib, "dinput8.lib")

class Input : public Object
{
	friend class InputSystem;
public:
	Input() = default;
	virtual ~Input() = default;
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator = (const Input&) = delete;
	Input& operator = (Input&&) = delete;
public:
	bool GetMouseDown(MOUSE_BUTTON_TYPE::eType type)
	{
		return _mouseDown[type];
	}
	bool GetMouseHold(MOUSE_BUTTON_TYPE::eType type)
	{
		return _mouseHold[type];
	}
	bool GetMouseUp(MOUSE_BUTTON_TYPE::eType type)
	{
		return _mouseUp[type];
	}
	const FVector2D& GetMouseWorldPos() const
	{
		return _mouseWorldPos;
	}
	const FVector2D& GetMousePos() const
	{
		return _mousePos;
	}
	const FVector2D& GetMouseMove() const
	{
		return _mouseMove;
	}
public:
	bool Init();
	void Tick(float deltaTime);
	bool GetKeyState(uint8 key);
	virtual void Destroy() override;
private:
	bool InitInput();
	bool UpdateKeyBoard();
	bool UpdateMouse();
	void UpdateInput(float deltaTime);
	void UpdateMousePos(float deltaTime);
	uint8 ConvertKey(uint8& key);
private:
	HINSTANCE _hInst = 0;
	HWND	  _hWnd	 = 0;
	ComPtr<IDirectInput8> _input = nullptr;				
	ComPtr<IDirectInputDevice8> _keyboard = nullptr;	// 키보드와 연결돼서 키보드 입력 상태를 가져오는 객체
	ComPtr<IDirectInputDevice8> _mouse = nullptr;		// 마우스와 연결돼서 입력 상태를 가져오는 객체 
	eInputSystemType _type = eInputSystemType::DINPUT;
	uint8 _keyState[256] = {};
	DIMOUSESTATE _mouseState = {};

	bool _mouseDown[MOUSE_BUTTON_TYPE::END] = {};
	bool _mouseHold[MOUSE_BUTTON_TYPE::END] = {};
	bool _mouseUp[MOUSE_BUTTON_TYPE::END] = {};

	FVector2D _mousePos;
	FVector2D _mouseWorldPos;
	FVector2D _mouseMove;
	bool	_mouseCompute = true;

};
#pragma once
#include "Widget.h"
#include <functional>

class Button : public Widget
{
public: 
	Button();
	virtual ~Button();
protected:
	FUIBrush _brush[UI_BUTTON_STATE::END];
	UI_BUTTON_STATE::Type _buttonState = UI_BUTTON_STATE::END;
	std::function<void()> _callBackFunc[UI_BUTTON_EVENT_STATE::END];
public:
	void SetTexture(UI_BUTTON_STATE::Type type, const std::string& name);
	void SetTexture(UI_BUTTON_STATE::Type type, const std::string& name, const std::wstring& fileName);
	void SetTexture(UI_BUTTON_STATE::Type type, Ptr<class Texture> texture);
	void SetOpacity(UI_BUTTON_STATE::Type type, float op);
	void SetTint(UI_BUTTON_STATE::Type type, float r, float g, float b, float a);

	void SetBrushAnimEnable(UI_BUTTON_STATE::Type type, bool enable);
	void AddBrushFrame(UI_BUTTON_STATE::Type type, const FVector2D& start, const FVector2D& size);
	void AddBrushFrame(UI_BUTTON_STATE::Type type, float startX, float startY, float sizeX, float sizeY);
	void SetCurrentFrame(UI_BUTTON_STATE::Type type, int32 frame);
	void SetAnimationPlayTime(UI_BUTTON_STATE::Type type, float time);
	void SetAnimationPlayRate(UI_BUTTON_STATE::Type type, float rate);
public:
	virtual bool Init(int32 id, const std::string& name, Weak<class Level> level);
	virtual void Tick(float deltaTime);
	virtual void Render();
	virtual void Render(const FVector3D& pos);
public:
	virtual Ptr<Widget> MouseHit(const FVector2D& pos);
	virtual bool IsInteractable();
	virtual void OnHovered();
	virtual void OnUnHovered();
	virtual void OnClick();
	virtual void OnPressed();
	virtual void Destroy();
private:
	void ExcuteEvent(UI_BUTTON_EVENT_STATE::Type type);
public:
	template<typename T>
	void BindAction(UI_BUTTON_EVENT_STATE::Type type, T&& func)
	{
		_callBackFunc[type] = std::forward<T>(func);
	}

	template<typename T>
	void BindAction(UI_BUTTON_EVENT_STATE::Type type, T* obj, void(T::* memFunc)())
	{
		_callBackFunc[type] = std::bind(memFunc, obj);
	}
};
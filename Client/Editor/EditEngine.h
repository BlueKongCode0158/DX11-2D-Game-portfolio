#pragma once
#include "../Common/Singleton.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"
#include <functional>

enum class ePopUpType
{
	Error,
	Warning,
	Info,
	Confirm,
	End
};

class EditEngine : public Singleton<EditEngine>
{
	DECLARE_SINGLETON(EditEngine);
public:
	bool Init();
	virtual void Destroy() override;
	bool Run();
	void Show(ePopUpType type, const std::string& msg);
	void ShowError(const std::string& msg);
	void ShowWarning(const std::string& msg);
	void ShowInfo(const std::string& msg);
	void ShowConfirm(const std::string& msg);
public:
	Ptr<class ViewportRenderTarget> GetViewportRenderTarget()
	{
		return _viewportRenderTarget;
	}
private:	
	void Logic(); //로직 실행 함수
	void Tick(float deltaTime); //매 프레임 들어오는 함수
	void Collision(float deltaTime);
	void Render(float deltaTime);
private:
	void RenderDockSpace();
private:
	float _clearColor[4] = { 0,0,0,1 };
	std::unordered_map<std::string, Ptr<class EditorUI>> _edits;
	std::unordered_map<std::string, std::function<void()>> _actorCreateFuncs;
	Ptr<class ViewportRenderTarget> _viewportRenderTarget;
	Ptr<class PopUPUI> _popUp;
private:
	template<typename T>
	Ptr<T> CreateEditorUI(const std::string& name)
	{
		Ptr<T> it = FindEditorUI<T>(name);
		if (it)
		{
			return it;
		}

		it = New<T>();
		if (!it->Init(name))
		{
			DESTROY(it);
			return nullptr;
		}
		_edits[name] = it;
		return it;
	}
public:
	template<typename T>
	Ptr<T> FindEditorUI(const std::string& name)
	{
		auto it = _edits.find(name);
		if (_edits.end() == it)
		{
			return nullptr;
		}
		return Cast<EditorUI, T>(it->second);
	}
};
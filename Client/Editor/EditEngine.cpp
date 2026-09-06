#include "pch.h"
#include "EditEngine.h"

#include "../Core/GameEngine.h"
#include "../Core/Device.h"
#include "../Core/TimeManager.h"

#include "../World/World.h"
#include "../Render/RenderManager.h"

#include "EditorUI/EditorUI.h"
#include "EditorUI/MainMenuUI.h"
#include "EditorUI/OutLinerUI.h"
#include "EditorUI/InspectorUI.h"
#include "EditorUI/PopUpUI.h"

#include "EditorUI/SaveUI.h"
#include "EditorUI/LoadUI.h"
#include "EditorUI/ViewportUI.h"
#include "EditorUI/AnimationUI.h"
#include "EditorUI/ParticleEditorUI.h"
#include "EditorUI/RenderDebugUI.h"

#include "EditorUI/ViewportRenderTarget.h"
#include "../Client.h"

bool EditEngine::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.f;
	style.Colors[ImGuiCol_WindowBg].w = 1.f;

	ImGui_ImplWin32_Init(GameEngine::Instance().GetHWND());
	ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

	g_isRun = true;

	_viewportRenderTarget = New<ViewportRenderTarget>();
	_viewportRenderTarget->Init(1280, 720);

	CreateEditorUI<MainMenuUI>("MainMenu");
	CreateEditorUI<OutLinerUI>("OutLiner");
	CreateEditorUI<InspectorUI>("Inspector");
	CreateEditorUI<ViewportUI>("Viewport");
	CreateEditorUI<SaveUI>("Save");
	CreateEditorUI<LoadUI>("Load");
	CreateEditorUI<AnimationUI>("Animation");
	CreateEditorUI<ParticleEditorUI>("Particle");
	CreateEditorUI<RenderDebugUI>("Render Debug");
	return true;
}

bool EditEngine::Run()
{
	MSG msg = {};
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

void EditEngine::Show(ePopUpType type, const std::string& msg)
{
	_popUp->Show(type, msg);
}

void EditEngine::ShowError(const std::string& msg)
{
	_popUp->ShowError(msg);
}

void EditEngine::ShowWarning(const std::string& msg)
{
	_popUp->ShowWarning(msg);
}

void EditEngine::ShowInfo(const std::string& msg)
{
	_popUp->ShowInfo(msg);
}

void EditEngine::ShowConfirm(const std::string& msg)
{
	_popUp->ShowConfirm(msg);
}

void EditEngine::Logic()
{
	float deltaTime = TimeManager::Instance().Tick();
	Tick(deltaTime);
	Collision(deltaTime);
	Render(deltaTime);
}

void EditEngine::Tick(float deltaTime)
{
	GameEngine::Instance().Tick(deltaTime);
}

void EditEngine::Collision(float deltaTime)
{
	GameEngine::Instance().Collision(deltaTime);
}

void EditEngine::Render(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	_viewportRenderTarget->Begin(_clearColor);
	GameEngine::Instance().GetWorld()->Render(deltaTime);
	RenderManager::Instance().Render(deltaTime);
	GameEngine::Instance().GetWorld()->RenderUI(deltaTime);
	_viewportRenderTarget->End();

	Device::Instance().ClearBackBuffer(_clearColor);
	Device::Instance().SetBackBufferTarget();

	RenderDockSpace();

	for (auto& it : _edits)
	{
		if (!it.second->IsEnable() || !it.second->IsActive())
		{
			continue;
		}
		it.second->Render(deltaTime);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	Device::Instance().Present();
}

void EditEngine::RenderDockSpace()
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	
	bool open = ImGui::Begin("EditorDockSpace", nullptr, windowFlags);
	ImGui::PopStyleVar(2);

	if (open)
	{
		ImGuiID dockspaceID = ImGui::GetID("MainDockspace");
		if (dockspaceID != 0)
		{
			ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None);
		}
	}

	ImGui::End();
}

void EditEngine::Destroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DESTROY(_viewportRenderTarget);
	DESTROY(_popUp);
}

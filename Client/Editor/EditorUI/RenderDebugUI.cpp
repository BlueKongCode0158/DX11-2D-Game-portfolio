#include "pch.h"
#include "RenderDebugUI.h"
#include "../EditEngine.h"
#include "../../Render/RenderManager.h"

RenderDebugUI::RenderDebugUI()
{
}

RenderDebugUI::~RenderDebugUI()
{
}

bool RenderDebugUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	return true;
}

void RenderDebugUI::Tick(float deltaTime)
{
	EditorUI::Tick(deltaTime);
}

void RenderDebugUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	if (!IsEnable())
	{
		return;
	}

#ifdef _EDITOR
	ImGui::Begin(_name.c_str());
	bool scene = RenderManager::Instance().GetShowScene();
	bool light = RenderManager::Instance().GetLightScene();

	if (ImGui::Checkbox("Scene RT", &scene))
	{
		RenderManager::Instance().SetShowScene(scene);
	}
	if (ImGui::Checkbox("Light RT", &light))
	{
		RenderManager::Instance().SetShowLight(light);
	}
#endif // !_EDITOR
	ImGui::End();
}
	
void RenderDebugUI::Destroy()
{
	EditorUI::Destroy();
}

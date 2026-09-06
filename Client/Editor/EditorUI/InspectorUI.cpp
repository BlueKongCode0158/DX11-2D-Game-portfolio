#include "pch.h"
#include "InspectorUI.h"
#include "../EditEngine.h"

#include "../../Object/Object.h"

InspectorUI::InspectorUI()
{
}

InspectorUI::~InspectorUI()
{
}

bool InspectorUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(true);
	return true;
}

void InspectorUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	ImGui::Begin(_name.c_str());

	if (!GetTarget())
	{
		ImGui::Text("Object is Null");
		ImGui::End();
		return;
	}
	GetTarget()->DrawInspector();
	ImGui::End();
}

void InspectorUI::Destroy()
{
	EditorUI::Destroy();
}

void InspectorUI::SetTarget(Ptr<class Object> target)
{
	_target = target;
}

Ptr<class Object> InspectorUI::GetTarget() const
{
	return Lock<class Object>(_target);
}

void InspectorUI::DrawWorldTransform()
{
}

void InspectorUI::DrawRelativeTransform()
{
}

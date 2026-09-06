#include "pch.h"
#include "OutLinerUI.h"
#include "InspectorUI.h"
#include "../EditEngine.h"

#include "../../Core/GameEngine.h"
#include "../../Object/Actor.h"

#include "../../World/World.h"
#include "../../World/Level.h"

#include "../../Component/Component.h"
#include "../../Component/SceneComponent.h"
#include "../../Component/ActorComponent.h"

OutLinerUI::OutLinerUI()
{
}

OutLinerUI::~OutLinerUI()
{
}

bool OutLinerUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(true);
	return true;
}

void OutLinerUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel();
	if (nullptr == level)
	{
		return;
	}

	ImGui::Begin(_name.c_str());
	auto& actors = level->GetActors();
	for (auto& actor : actors)
	{
		DrawActor(actor.second);
	}

	DrawLevelComponents(level);
	ImGui::End();
}

void OutLinerUI::Destroy()
{
	EditorUI::Destroy();

}

void OutLinerUI::DrawActor(Ptr<class Actor> actor)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	std::string label = std::format("{} | {}", actor->GetActorID(), actor->GetName());

	Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");
	if (!inspector)
	{
		return;
	}

	if (inspector->GetTarget() == actor)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		inspector->SetTarget(actor);
	}

	if (open)
	{
		DrawSceneComponent(actor->GetRoot());
		ImGui::TreePop();
	}


}

void OutLinerUI::DrawSceneComponent(Ptr<class SceneComponent> comp)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	std::string label = comp->GetName();

	auto& childs = comp->GetChilds();

	if (childs.size() < 1)
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");
	if (!inspector)
	{
		return;
	}

	if (inspector->GetTarget() == comp)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (ImGui::IsItemClicked())
	{
		inspector->SetTarget(comp);
	}

	if (open && childs.size() > 0)
	{
		for (auto& child : childs)
		{
			DrawSceneComponent(child.second);
		}
		ImGui::TreePop();
	}

}

void OutLinerUI::DrawLevelComponents(Ptr<class Level> level)
{
	const auto& comps = level->GetLevelComponents();
	if (comps.empty())
	{
		return;
	}

	if (!ImGui::CollapsingHeader("Level Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	for (auto& it : comps)
	{
		// SceneComponent 면 트리(자식 포함)로, 그 외 ActorComponent 류는 단일 노드로.
		Ptr<SceneComponent> sceneComp = Cast<Component, SceneComponent>(it.second);
		if (sceneComp)
		{
			DrawSceneComponent(sceneComp);
		}
		else
		{
			DrawComponentLeaf(it.second);
		}
	}
}

void OutLinerUI::DrawComponentLeaf(Ptr<class Component> comp)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;

	Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");
	if (inspector && inspector->GetTarget() == comp)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::TreeNodeEx(comp->GetName().c_str(), flags);

	if (ImGui::IsItemClicked() && inspector)
	{
		inspector->SetTarget(comp);
	}
}

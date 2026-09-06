#include "pch.h"
#include "ViewportUI.h"
#include "InspectorUI.h"
#include "ViewportRenderTarget.h"

#include "../EditEngine.h"

#include "../../Object/Actor.h"
#include "../../Core/GameEngine.h"
#include "../../Component/SceneComponent.h"
#include "../../World/World.h"
#include "../../World/Level.h"

ViewportUI::ViewportUI()
{
}

ViewportUI::~ViewportUI()
{
}

bool ViewportUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(true);

	return true;
}

void ViewportUI::Tick(float deltaTime)
{
	EditorUI::Tick(deltaTime);
}

void ViewportUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	//ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
	ImGui::Begin(_name.c_str());

	bool viewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	bool viewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
	Ptr<ViewportRenderTarget> viewportTarget = EditEngine::Instance().GetViewportRenderTarget();

	ImVec2 size = ImGui::GetContentRegionAvail();
	if (size.x > 1.f && size.y > 1.f && viewportTarget)
	{
		ImGui::Image(reinterpret_cast<ImTextureID>(viewportTarget->GetSRV().Get()), size);
	}

	ImVec2 imageMin = ImGui::GetItemRectMin();
	ImVec2 imageSize = ImGui::GetItemRectSize();

	HandleGizmoShortcut(viewportFocused, viewportHovered);
	DrawGizmo(imageMin, imageSize);

	ImGui::End();
}

void ViewportUI::Destroy()
{
	EditorUI::Destroy();
}

void ViewportUI::HandleGizmoShortcut(const bool& viewportFocused, const bool& viewportHovered)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!viewportFocused && !viewportHovered)
	{
		return;
	}

	if (ImGui::GetIO().WantTextInput)
	{
		return;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Q))
	{
		_gizmoEnabled = false;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_W))
	{
		_gizmoEnabled = true;
		_gizmoOperation = ImGuizmo::TRANSLATE;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_E))
	{
		_gizmoEnabled = true;
		_gizmoOperation = ImGuizmo::ROTATE;
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_R))
	{
		_gizmoEnabled = true;
		_gizmoOperation = ImGuizmo::SCALE;
	}
}

void ViewportUI::DrawGizmo(const ImVec2& imageMin, const ImVec2& imageSize)
{
	if (!_gizmoEnabled)
	{
		return;
	}

	if (imageSize.x <= 1.f || imageSize.y <= 1.f)
	{
		return;
	}

	Ptr<SceneComponent> target = GetSelectedSceneComponent();
	if (!target)
	{
		return;
	}

	Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel();
	if (!level)
	{
		return;
	}

	FMatrix view = level->GetViewMatrix();
	FMatrix proj = level->GetProjMatrix();
	FMatrix world = BuildWorldMatrix(target->GetWorldTransform());

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(imageMin.x, imageMin.y, imageSize.x, imageSize.y);

	bool manipulated = ImGuizmo::Manipulate(reinterpret_cast<float*>(&view), reinterpret_cast<float*>(&proj),
		_gizmoOperation, _gizmoMode, reinterpret_cast<float*>(&world));

	if (manipulated && ImGuizmo::IsUsing())
	{
		FVector3D position;
		FVector3D scale;
		FRotator rotation;

		world.ExtractPosition(position);
		world.ExtractScale(scale);
		world.ExtractEuler(rotation);

		target->SetWorldTransform(position, scale, rotation);
	}
}

Ptr<class SceneComponent> ViewportUI::GetSelectedSceneComponent()
{
	Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");

	if (!inspector)
	{
		return nullptr;
	}

	Ptr<Object> target = inspector->GetTarget();
	if (!target)
	{
		return nullptr;
	}

	Ptr<Actor> actor = Cast<Object, Actor>(target);
	if (actor)
	{
		return actor->GetRoot();
	}

	Ptr<SceneComponent> sceneComp = Cast<Object, SceneComponent>(target);
	if (sceneComp)
	{
		return sceneComp;
	}

	return nullptr;
}

FMatrix ViewportUI::BuildWorldMatrix(const FTransform& transform)
{
	FMatrix scale;
	FMatrix rotation;
	FMatrix translation;

	scale.Scaling(transform._scale);
	rotation.Rotation(transform._rotation);
	translation.Translation(transform._position);

	return scale * rotation * translation;
}

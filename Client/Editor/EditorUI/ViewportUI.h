#pragma once
#include "EditorUI.h"

#include "../../Editor/Imgui/imgui.h"
#include "../Imgui/ImGuizmo.h"

class ViewportUI : public EditorUI
{
public:
	ViewportUI();
	virtual ~ViewportUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
private:
	void HandleGizmoShortcut(const bool& viewportFocused, const bool& viewportHovered);
	void DrawGizmo(const ImVec2& imageMin, const ImVec2& imageSize);
private:
	Ptr<class SceneComponent> GetSelectedSceneComponent();
	FMatrix BuildWorldMatrix(const FTransform& transform);

private:
	bool _gizmoEnabled = false;
	ImGuizmo::OPERATION _gizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE _gizmoMode = ImGuizmo::WORLD;
};
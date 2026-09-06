#pragma once
#include "EditorUI.h"

class OutLinerUI : public EditorUI
{
public:
	OutLinerUI();
	virtual ~OutLinerUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
private:
	void DrawActor(Ptr<class Actor> actor);
	void DrawSceneComponent(Ptr<class SceneComponent> comp);
	void DrawLevelComponents(Ptr<class Level> level);	// 액터에 안 붙은 Level 직속 컴포넌트
	void DrawComponentLeaf(Ptr<class Component> comp);	// SceneComponent 트리가 아닌 단일 컴포넌트 노드
};
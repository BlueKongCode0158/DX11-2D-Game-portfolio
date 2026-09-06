#pragma once
#include "EditorUI.h"

class RenderDebugUI : public EditorUI
{
public:
	RenderDebugUI();
	virtual ~RenderDebugUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
};
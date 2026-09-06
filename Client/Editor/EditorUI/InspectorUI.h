#pragma once
#include "EditorUI.h"

class InspectorUI : public EditorUI
{
public:
	InspectorUI();
	virtual ~InspectorUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
public:
	void SetTarget(Ptr<class Object> target);
	Ptr<class Object> GetTarget() const;
private:
	void DrawWorldTransform();
	void DrawRelativeTransform();
private:
	Weak<class Object> _target;
};
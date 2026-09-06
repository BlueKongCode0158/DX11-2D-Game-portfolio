#pragma once
#include "../SceneComponent.h"

class WidgetComponent : public SceneComponent
{
public:
	WidgetComponent();
	virtual ~WidgetComponent();
protected:
	Ptr<class Widget> _widget;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetWidget(Ptr<class Widget> widget);
};
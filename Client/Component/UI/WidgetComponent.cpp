#include "pch.h"
#include "WidgetComponent.h"
#include "../../UI/Image.h"
#include "../../UI/Button.h"
#include "../../UI/TextBlock.h"
#include "../../UI/ProgressBar.h"
#include "../../UI/Widget.h"

WidgetComponent::WidgetComponent()
{
	_isRender = true;
}

WidgetComponent::~WidgetComponent()
{
}

bool WidgetComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	SetRenderLayerName("UI");

	_type = eComponentType::WIDGET;
	return true;
}

void WidgetComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
	if (_widget)
	{
		_widget->Tick(deltaTime);
	}
}

void WidgetComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);
	if (_widget)
	{
		_widget->Render(_world._position);
	}
}

void WidgetComponent::Destroy()
{
}

bool WidgetComponent::Save(std::ofstream& file)
{
	return true;
}

bool WidgetComponent::Load(std::ifstream& file)
{
	return true;
}

void WidgetComponent::SetWidget(Ptr<class Widget> widget)
{
	_widget = widget;
	_widget->SetOwnerActor(_widget->GetOwnerActor());
}

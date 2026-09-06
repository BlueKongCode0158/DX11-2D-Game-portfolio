#include "pch.h"
#include "RenderModifier2DComponent.h"

bool RenderModifier2DComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	return true;
}

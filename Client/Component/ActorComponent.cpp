#include "pch.h"
#include "ActorComponent.h"

ActorComponent::ActorComponent()
{
}

ActorComponent::~ActorComponent()
{
}

bool ActorComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	Component::Init(id, name, owner);
	_type = eComponentType::ACTOR;
	return true;
}

void ActorComponent::Tick(float deltaTime)
{
	Component::Tick(deltaTime);
}

void ActorComponent::Destroy()
{
}

bool ActorComponent::Save(std::ofstream& file)
{
	return true;
}

bool ActorComponent::Load(std::ifstream& file)
{
	return true;
}

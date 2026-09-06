#pragma once
#include "Component.h"

class ActorComponent : public Component
{
public:
	ActorComponent();
	virtual ~ActorComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
}; 
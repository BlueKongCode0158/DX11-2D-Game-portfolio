#pragma once
#include "SceneComponent.h"

class MeshComponent : public SceneComponent
{
public:
	MeshComponent();
	virtual ~MeshComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
protected:
	std::vector<Ptr<class Material>> _materialSlots;
};
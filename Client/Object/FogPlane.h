#pragma once
#include "Object/Actor.h"

class FogPlane : public Actor
{
public:
	FogPlane() = default;
	virtual ~FogPlane() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
private:
	Ptr<class FogPlaneComponent> _comp;
};
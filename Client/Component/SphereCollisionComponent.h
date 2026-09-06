#pragma once 
#include "CollisionComponent.h"

class SphereCollisionComponent : public CollisionComponent
{
public:
	SphereCollisionComponent() = default;
	virtual ~SphereCollisionComponent() = default;
public:
	virtual bool Init(int32 id, const std::string name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void DrawInspector() override;
public:
	virtual void Destroy() override;
	virtual bool Collision(Weak<CollisionComponent> dest);
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	const float GetRadius() { return _radius; }
	void SetRadius(float radius) { _radius = radius; }
private:
	float _radius = 0.f;
};
#pragma once
#include "Actor.h"

class CameraZone : public Actor
{
public:
	CameraZone() = default;
	virtual ~CameraZone() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual	void Tick(float deltaTime) override;
public:
	void SetTriggerSize(float x, float y);
public:
	void OnTriggerZone(Weak <class CollisionComponent> comp);
public:
	virtual void Destroy() override;
private:
	Ptr<class AABBCollisionComponent> _LockTrigger;
};
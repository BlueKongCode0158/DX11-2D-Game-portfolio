#pragma once
#include "Actor.h"

class TutorialDoor : public Actor
{
public:
	TutorialDoor() = default;
	virtual ~TutorialDoor() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
private:
	Ptr<class AABBCollisionComponent> _ColliderComp;
};
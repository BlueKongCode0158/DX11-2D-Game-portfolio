#pragma once
#include "Actor.h"

class AnimActor : public Actor
{
public:
	AnimActor() = default;
	virtual ~AnimActor() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual	void Tick(float deltaTime) override;
public:
	virtual void Destroy() override;
public:
	void AddAnimation(std::string clipName);
	void SetTint(const FVector4D& tint);
private:
	Ptr<class SpriteComponent> _animComp;
};
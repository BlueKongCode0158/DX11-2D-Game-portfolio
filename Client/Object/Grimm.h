#pragma once
#include "Pawn.h"

class Grimm : public Pawn
{
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Collision(float deltaTime);
public:
	virtual void Destroy() override;
private:

};
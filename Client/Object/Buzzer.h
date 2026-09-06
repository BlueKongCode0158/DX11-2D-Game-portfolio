#pragma once
#include "Pawn.h"

class Buzzer : public Pawn
{
public:
	Buzzer();
	virtual ~Buzzer();
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
public:
	virtual void Destroy() override;
private:
	Ptr<class PlayerTargetMonsterBoard> _board;
	Ptr<class MovementComponent>		_movement;
};
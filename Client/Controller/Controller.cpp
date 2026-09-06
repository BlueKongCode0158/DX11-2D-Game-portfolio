#include "pch.h"
#include "Controller.h"

bool Controller::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);
	SetEnable(false);

	return true;
}

void Controller::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
	// TODO : Logic
}

void Controller::Collision(float deltaTime)
{
	// TODO : Logic_Collistion
}

void Controller::Render(float deltaTime)
{
	// TODO : Logic_Render
}

void Controller::Destroy()
{
	Actor::Destroy();
}

Ptr<class Pawn> Controller::GetPawn() const
{
	return Lock<Pawn>(_pawn);
}

void Controller::SetPawn(Ptr<class Pawn> pawn)
{
	_pawn = pawn;
}

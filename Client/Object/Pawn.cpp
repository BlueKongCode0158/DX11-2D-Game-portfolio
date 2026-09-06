#include "pch.h"
#include "Pawn.h"
#include "Controller/Controller.h"

Pawn::Pawn()
{
}

Pawn::~Pawn()
{
}

bool Pawn::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);
	_type = eActorType::Pawn;

	return true;
}

void Pawn::Tick(float deltaTime)
{
	if (_controller)
	{
		_controller->Tick(deltaTime);
	}
	Actor::Tick(deltaTime);
}

void Pawn::Collision(float deltaTime)
{
	Actor::Collision(deltaTime);
}

void Pawn::Render(float deltaTime)
{
	Actor::Render(deltaTime);
}

void Pawn::Destroy()
{
	Actor::Destroy();

	DESTROY(_controller);
}

bool Pawn::Save(std::ofstream& file)
{
	Actor::Save(file);
	return true;
}

bool Pawn::Load(std::ifstream& file)
{
	Actor::Load(file);
	return true;
}

void Pawn::SetController(Ptr<class Controller> ctrl)
{
	if (_controller)
	{
		DESTROY(_controller);
	}
	_controller = ctrl;
}

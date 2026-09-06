#include "pch.h"
#include "MovementComponent.h"
#include "Input/InputSystem.h"
#include "Input/InputContext.h"
#include "Component/SceneComponent.h"
#include "Component/InputComponent.h"
#include "Controller/PlayerController.h"
#include "Object/Pawn.h"
#include "Common/LogManager.h"

bool MovementComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	ActorComponent::Init(id, name, owner);
	_speed = 1.f;
	_type = eComponentType::MOVEMENT;
	return true;
}

void MovementComponent::Tick(float deltaTime)
{
	ActorComponent::Tick(deltaTime);
	if (nullptr == _updateComponent)
	{
		return;
	}

	if (_moveAxis.Length() <= 0)
	{
		return;
	}

	FVector3D delta = _moveAxis * _speed * deltaTime * _speedRange;
	_updateComponent->AddRelativePosition(delta);
}

void MovementComponent::Destroy()
{
	ActorComponent::Destroy();
}

bool MovementComponent::Save(std::ofstream& file)
{
	return true;
}

bool MovementComponent::Load(std::ifstream& file)
{
	return true;
}

void MovementComponent::SetUpdateComponent(Ptr<class SceneComponent> updateComp)
{
	_updateComponent = updateComp;
}

Ptr<class SceneComponent> MovementComponent::GetUpdateComponent() const
{
	return _updateComponent;
}

void MovementComponent::SetMoveAxis(const FVector3D& moveAxis)
{
	_moveAxis = moveAxis;
	_moveAxis.Normalize();
}

void MovementComponent::AddMoveAxis(const FVector3D& moveAxis)
{
	_moveAxis += moveAxis;
	_moveAxis.Normalize();
}

void MovementComponent::Stop()
{
	_moveAxis = FVector3D::Zero;
}

#include "pch.h"
#include "PlayerController.h"
#include "Object/Player.h"
#include "Input/InputSystem.h"
#include "../Component/AI/AIComponent.h"
#include "../Component/AI/PlayerStateMachine.h"

bool PlayerController::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	AIController::Init(id, pos, scale, rot, name);

	_input = CreateActorComponent<InputComponent>("Input");
	_aiComponent->CreateAIStateMachine<PlayerStateMachine>();
	InputSystem::Instance().RegisterInputComponent(GetActorID(), _input);

	return true;
}

void PlayerController::Tick(float deltaTime)
{
	AIController::Tick(deltaTime);
}

void PlayerController::Render(float deltaTime)
{
	AIController::Render(deltaTime);
}

void PlayerController::Collision(float deltaTime)
{
	AIController::Collision(deltaTime);
}

void PlayerController::Destroy()
{
	AIController::Destroy();
	InputSystem::Instance().RemoveInputComponent(GetActorID(), _input->GetComponentID());
}

Ptr<class InputComponent> PlayerController::GetInputComponent() const
{
	return _input;;
}

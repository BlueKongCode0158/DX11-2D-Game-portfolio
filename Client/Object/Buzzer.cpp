#include "pch.h"
#include "Buzzer.h"

#include "../Component/Animation/SpriteComponent.h"
#include "../Component/AABBCollisionComponent.h"
#include "../Component/SphereCollisionComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/AI/AIComponent.h"
#include "../Component/AI/AIBoard.h"
#include "../Controller/AIController.h"

#include "../World/Level.h"
#include "../World/World.h"

Buzzer::Buzzer()
{
}

Buzzer::~Buzzer()
{
}

bool Buzzer::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Pawn::Init(id, pos, scale, rot, name);

	_controller = GetLevel()->SpawnActor<AIController>("Monster_Controller", pos, scale, rot);
	_controller->SetPawn(This<Buzzer>());
	_controller;

	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("Anim");
	meshComp->AttachToComponent(_root);
	meshComp->SetUseAnimationFrameTransform(true);

	meshComp->AddAnimSequenceAuto("tk2d_1150::Idle");
	meshComp->AddAnimSequenceAuto("tk2d_1150::Chase");
	meshComp->AddAnimSequenceAuto("tk2d_1150::Fly");
	meshComp->AddAnimSequenceAuto("tk2d_1150::Startle");
	meshComp->AddAnimSequenceAuto("tk2d_1150::TurnToIdle");
	meshComp->AddAnimSequenceAuto("tk2d_1150::TurnToFly");
	meshComp->AddAnimSequenceAuto("tk2d_1150::Death Air");
	meshComp->ChangeAnimation("tk2d_1150::Fly");

	Ptr<SphereCollisionComponent> PlayerDetectionTrigger = CreateSceneComponent<SphereCollisionComponent>("PlayerDetectionTrigger");
	PlayerDetectionTrigger->SetRadius(200.f);
	PlayerDetectionTrigger->AttachToComponent(_root);
	PlayerDetectionTrigger->SetCollisionProfile("Monster");

	_movement = CreateActorComponent<MovementComponent>("Movement");
	_movement->SetSpeed(64.f * 3.f);
	_movement->SetUpdateComponent(_root);

	Ptr<AIController> controller = Cast<Controller, AIController>(_controller);
	if (!controller)
	{
		return false;
	}

	
	//Ptr<MachineBase> machine = controller->GetAIStateMachine();
	//_board = machine->GetAIBoard<PlayerTargetMonsterBoard>();

	//if (Lock<PlayerTargetMonsterBoard>(_board))
	//{
	//	Lock<PlayerTargetMonsterBoard>(_board)->_actor = This<Actor>();
	//}

	return true;
}

void Buzzer::Tick(float deltaTime)
{
	Pawn::Tick(deltaTime);
	
}

void Buzzer::Collision(float deltaTime)
{
	Pawn::Collision(deltaTime);
}

void Buzzer::Render(float deltaTime)
{
	Pawn::Render(deltaTime);
}

void Buzzer::Destroy()
{
	Pawn::Destroy();
}

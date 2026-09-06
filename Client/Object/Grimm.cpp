#include "pch.h"
#include "Grimm.h"

#include "../Component/Animation/SpriteComponent.h"

bool Grimm::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Pawn::Init(id, pos, scale, rot, name);

	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("Anim");
	meshComp->AttachToComponent(_root);
	meshComp->SetUseAnimationFrameTransform(true);

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::TeleToIdle");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Tele Out");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Tele In");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Roar Antic");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Roar");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::LookBack");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::LookBackReturn");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Idle");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::G Dash");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::G Dash Antic");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Air Dash");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Air Dash Antic");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Death Stun");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Cast Antic");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Cast");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Cast Return");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Bow");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Bow Return");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Balloon Antic");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Balloon");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Absorb Start");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Absorb");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Absorb End");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Capespike Cast");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Capespike End");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Explode");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Explode Antic");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Slash Antic");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Slash 1");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Slash 2");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Slash 3");

	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Uppercut");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Uppercut Antic");
	meshComp->AddAnimSequenceAuto("tk2d_sharedassets392_131::Uppercut End");

	meshComp->ChangeAnimation("tk2d_sharedassets392_131::Idle");

	Ptr<SpriteComponent> handAnim = CreateSceneComponent<SpriteComponent>("hand");
	handAnim->AttachToComponent(_root);
	handAnim->SetUseAnimationFrameTransform(true);

	handAnim->AddAnimSequenceAuto("tk2d_sharedassets392_131::Hand Click");
	handAnim->AddAnimSequenceAuto("tk2d_sharedassets392_131::Hand In");
	handAnim->AddAnimSequenceAuto("tk2d_sharedassets392_131::Hand Out");
	handAnim->SetEnable(false);

	return true;
}

void Grimm::Tick(float deltaTime)
{
	Pawn::Tick(deltaTime);
}

void Grimm::Render(float deltaTime)
{
	Pawn::Render(deltaTime);
}

void Grimm::Collision(float deltaTime)
{
	Pawn::Collision(deltaTime);
}

void Grimm::Destroy()
{
	Pawn::Destroy();
}

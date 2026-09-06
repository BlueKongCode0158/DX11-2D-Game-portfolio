#include "pch.h"
#include "Monster.h"
#include "World/Level.h"
#include "Component/AABBCollisionComponent.h"
#include "Component/SphereCollisionComponent.h"
#include "Component/StaticMeshComponent.h"
#include "../Component/Animation/SpriteComponent.h"

bool Monster::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Pawn::Init(id, pos, scale, rot, name);
	_type = eActorType::Monster;

	Ptr<SpriteComponent> meshComp = CreateSceneComponent<SpriteComponent>("ProbMesh");
	meshComp->SetShader("SpriteMeshShader");
	meshComp->CreateAnimation();
	meshComp->SetRelativeScale(100.f, 100.f, 1.f);

	return true;
}

void Monster::Tick(float deltaTime)
{
	Pawn::Tick(deltaTime);
}

void Monster::Collision(float deltaTime)
{
	Pawn::Collision(deltaTime);
}

void Monster::Render(float deltaTime)
{
	Pawn::Render(deltaTime);
}

void Monster::Destroy()
{
	Pawn::Destroy();
}

bool Monster::Save(std::ofstream& file)
{
	Pawn::Save(file);
	return true;
}

bool Monster::Load(std::ifstream& file)
{
	Pawn::Load(file);
	return true;
}

void Monster::SetTarget(Ptr<class Player> player)
{
	_target = player;
}

void Monster::BlockCallback(Weak<class CollisionComponent> comp)
{
	Ptr<Level> level = GetLevel();
	if (nullptr == level)
		return;

	level->RemoveActor(_id);
}

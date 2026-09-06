#include "pch.h"
#include "TutorialDoor.h"

#include "../Component/AABBCollisionComponent.h"
#include "../Component/Animation/SpriteComponent.h"

bool TutorialDoor::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	return true;
}

void TutorialDoor::Tick(float deltaTime)
{

}

void TutorialDoor::Destroy()
{
	Actor::Destroy();
}

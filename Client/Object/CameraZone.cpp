#include "pch.h"
#include "CameraZone.h"

#include "../Component/AABBCollisionComponent.h"
#include "../Component/EdgeCollisionComponent.h"
#include "../Component/CameraComponent.h"

#include "../World/World.h"
#include "../World/Level.h"

bool CameraZone::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);

	_LockTrigger = CreateSceneComponent<AABBCollisionComponent>("Lock");
	_LockTrigger->SetCollisionProfile("CameraZone");
	_LockTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &CameraZone::OnTriggerZone);
	_LockTrigger->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &CameraZone::OnTriggerZone);
	_LockTrigger->AttachToComponent(_root);

	return true;
}

void CameraZone::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void CameraZone::SetTriggerSize(float x, float y)
{
	_LockTrigger->SetBoxSize(x, y);
}

void CameraZone::OnTriggerZone(Weak<class CollisionComponent> comp)
{
	Ptr<Level> level = GetLevel();
	if (!level)
	{
		return;
	}

	Ptr<CameraComponent> cam = level->GetMainCamera();
	if (!cam)
	{
		return;
	}
	FLocalBound2D lock = _LockTrigger->GetLocalBound();
	cam->SetCameraLock(lock);
}

void CameraZone::Destroy()
{
	Actor::Destroy();
}

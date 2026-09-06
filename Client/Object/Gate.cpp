#include "pch.h"
#include "Gate.h"

#include "../Core/GameEngine.h"
#include "../World/World.h"
#include "../Component/AABBCollisionComponent.h"

bool Gate::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);

	_aabbCollider = CreateSceneComponent<AABBCollisionComponent>("GateCollider");
	_aabbCollider->SetCollisionProfile("Gate");
	_aabbCollider->SetBoxSize(128.f, 256.f);
	_aabbCollider->SetCollisionCallBack(eCollisionState::COLLISION_STATE_OVERLAP, this, &Gate::OnGate);
	_aabbCollider->SetCollisionCallBack(eCollisionState::COLLISION_STATE_BLOCK, this, &Gate::BlockGate);
	_aabbCollider->AttachToComponent(_root);

	return true;
}

void Gate::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Gate::Destroy()
{
	Actor::Destroy();
}

void Gate::OnGate(Weak<class CollisionComponent> comp)
{
	Ptr<World> world = GameEngine::Instance().GetWorld();

	if (!world)
	{
		return;
	}

	// 즉각적인 이동
	if (GATETYPE::IMMEDIATE == _gateType)
	{
		world->SetNextLevelType(_levelType);
	} 
}

// 애니메이션 출력
void Gate::BlockGate(Weak<class CollisionComponent> comp)
{
	if (GATETYPE::REACTIVE == _gateType)
	{

	}
}

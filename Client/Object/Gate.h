#pragma once
#include "Actor.h"

// 플레아어 지역 이동
// UI 붙은 버전 - 안 붙은 버전 어떻게 구분할까
// 플래그 하나 붙여서 붙은 건 플레이어 입력 반응 있을 때 넘어가게 할까

enum GATETYPE
{
	IMMEDIATE,
	REACTIVE,
	GATE_END
};

class Gate : public Actor
{
public:
	Gate() = default;
	virtual ~Gate() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
public:
	void OnGate(Weak<class CollisionComponent> comp);
	void BlockGate(Weak<class CollisionComponent> comp);
public:
	void SetLevelType(eLevelType eLevelType)
	{
		_levelType = eLevelType;
	}
	void SetGateType(GATETYPE gateType)
	{
		_gateType = gateType;
	}
private:
	Ptr<class AABBCollisionComponent> _aabbCollider;
	eLevelType _levelType = eLevelType::LEVEL_END;
private:
	GATETYPE _gateType = GATETYPE::GATE_END;
};
#pragma once
#include "Pawn.h"


// 상태머신 따로 뺄 것.
class Crawler : public Pawn
{
public:
	Crawler();
	virtual ~Crawler();
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
public:
	virtual void Destroy() override;
private:
	void BlockLeftWall(Weak<class CollisionComponent> comp);
	void BlockRightWall(Weak<class CollisionComponent> comp);
	void AttackedMonster(Weak<class CollisionComponent> comp);
private:
	void ExitTurnAnim();
	void ExitDeadAirAnim();
	void DeadMonster();
private:
	Ptr<class AABBCollisionComponent>	_col;
	Ptr<class MovementComponent>	_movement;
	int32							_moveDir = 1;
private:
	int32							_Hp = 3;
};
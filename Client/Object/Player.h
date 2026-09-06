#pragma once
#include "Pawn.h"

class Player : public Pawn
{
public :
	Player() = default;
	virtual ~Player() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Collision(float deltaTime);
public:
	virtual void Destroy() override;
public:
	void SetupStateMachine();
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void MoveRight(float deltaTime);
	void MoveLeft(float deltaTime);
	void MoveStop(float deltaTime);
public:
	void Attack(float deltaTime);
public:
	void Jump(float deltaTime);
	void JumpStop(float deltaTime);
public:
	void LookDown(float deltaTime);
	void LookUp(float deltaTime);
public:
	void Dash(float deltaTime);
public:
	Ptr<class PlatformMovementComponent> GetMovement();
public:
	bool LookRight();
private:
	void ShakeCameraTest(float deltaTime);
private:
	void OnGround(Weak<class CollisionComponent> comp);
	void OnCeiling(Weak<class CollisionComponent> comp);
	void OnLeftWall(Weak<class CollisionComponent> comp);
	void OnRightWall(Weak<class CollisionComponent> comp);
private:
	void ExitWingAnim();
	void ExitDashEffectAnim();
	void DoubleJumpEffect();
	void DashEffect();
	void ExitDoubleJumpAnim();
	void StopAnimation();
private:
	Ptr<class PlatformMovementComponent> _movement;
	Ptr<class AABBCollisionComponent> _col;
	Ptr<class Light2DComponent> _frontLight;
	Ptr<class Light2DComponent> _backLight;
	Ptr<class SoundComponent> _sound;
private:
	Weak<class PlayerBoard> _board;
	int32 _timerID = -1;
private:
	float _graoundY = 0.f;
};
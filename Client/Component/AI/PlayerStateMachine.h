#pragma once
#include "MachineBase.h"

class PlayerStateMachine : public MachineBase
{
public:
	PlayerStateMachine() {};
	virtual ~PlayerStateMachine() {};
public:
	virtual void Init(Ptr<class AIComponent> owner) override;
	virtual void Destroy() override;
private:
	Ptr<class PlayerBoard> GetBoard();
	Ptr<class Player> GetPlayer();
private:
	void OnIdleEnter(float deltaTime);
	void OnMoveEnter(float deltaTime);
	void OnJumpEnter(float deltaTime);
	void OnFallEnter(float deltaTime);
	void OnWallSlideEnter(float deltaTime);
	void OnAttackEnter(float deltaTime);
	void OnAttackAltEnter(float deltaTime);
	void OnAttackDownEnter(float deltaTime);
	void OnAttackUpEnter(float deltaTime);
	void OnDoubleJumpEnter(float deltaTime);
	void OnDashEnter(float deltaTime);
	void OnWallEnter(float deltaTime);
private:
	void OnIdleTick(float deltaTime);
	void OnMoveTick(float deltaTime);
	void OnJumpTick(float deltaTime);
	void OnFallTick(float deltaTime);
	void OnWallSlideTick(float deltaTime);
	void OnAttackTick(float deltaTime);
	void OnAttackAltTick(float deltaTime);
	void OnAttackDownTick(float deltaTime);
	void OnAttackUpTick(float deltaTime);
	void OnDoubleJumpTick(float deltaTime);
	void OnDashTick(float deltaTime);
	void OnWallTick(float deltaTime);
private:
	void OnIdleExit(float deltaTime);
	void OnMoveExit(float deltaTime);
	void OnJumpExit(float deltaTime);
	void OnFallExit(float deltaTime);
	void OnWallSlideExit(float deltaTime);
	void OnAttackExit(float deltaTime);
	void OnAttackAltExit(float deltaTime);
	void OnAttackDownExit(float deltaTime);
	void OnAttackUpExit(float deltaTime);
	void OnDoubleJumpExit(float deltaTime);
	void OnDashExit(float deltaTime);
	void OnWallExit(float deltaTime);
private:
	bool IsRun();
private:
	bool CondCanMove();
	bool CondCanIdle();
	bool CondCanJump();
	bool CondIsFalling();
	bool CondIsGrounded();
	bool CondCanWallSlide();
	bool CondNotWallSlide();
	bool CondCanAttack();
	bool CondAttackDone();
	bool CondCanAttackDown();
	bool CondCanAttackUp();
	bool CondCanDoubleJump();
	bool CondCanDash();
	bool CondIsFinishDash();
	bool CondIsWall();
	bool CondIsNotWall();
};
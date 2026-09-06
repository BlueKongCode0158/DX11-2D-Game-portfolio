#pragma once
#include "../../Object/Object.h"

class AIBoard : public Object
{
public:
	AIBoard();
	virtual ~AIBoard();
public:
	virtual void Destroy() override;
};

class PlayerTargetMonsterBoard : public AIBoard
{
public:
	PlayerTargetMonsterBoard() {};
	virtual ~PlayerTargetMonsterBoard() {};
public:
	Ptr<class Actor>	_target;
	FVector3D			_moveDir;
public:
	bool _isTarget	= false;
	bool _isWall	= false;
public:
	Weak<class Actor> _actor;
};

class PlayerBoard : public AIBoard
{
public:
	PlayerBoard() {};
	virtual ~PlayerBoard() {};
public:
	bool _isPressLeft		= false;	// vk_left
	bool _isPressRight		= false;	// vk_right
	bool _isPressUp			= false;	// vk_up
	bool _isPressDown		= false;	// vk_down
	bool _isJump			= false;
	bool _isDoubleJump		= false;
	bool _isAttack			= false;
	bool _isGrounded		= false;
	bool _isFall			= false;
public:
	bool _isWallLeft		= false;	
	bool _isWallRight		= false;	
	bool _isWallSliding		= false;	
	bool _isAttackDone		= false;
public:
	bool _isFacingRight		= false;
public:
	bool _isDash			= false;
	float _dashTime			= 0.f;
	float _dashCoolTime	= 0.f;
	int _dashDir			= 1;
public:
	const float DASH_DURATION = 0.35f;
	const float DASH_SPEED_RANGE = 3.f;
	const float DASH_COOLDOWN = 0.6f;
public:
	Weak<class Actor> _actor;
};
#pragma once
#include "MovementComponent.h"

class PlatformMovementComponent : public MovementComponent
{
public:
	PlatformMovementComponent();
	virtual ~PlatformMovementComponent();
public:
	virtual void Tick(float deltaTime) override;
	void SetVelocity(const FVector3D& vel);
	void AddVelocity(const FVector3D& vel);
	void AddVelocity(const float& vel);
	void ZeroVerticalVelocity();
public:
	const FVector3D& GetVelocity() const;
	bool IsGrounded() const;
	bool IsWall() const;
public:
	void SetGravityEnabled(bool enabled);
	void SetGravity(float gravity);
	void SetMaxFallSpeed(float speed);
public:
	void StartJump();
	void StartDoubleJump();
	void StopJump();
	bool IsJumping();
	void SetGrounded(bool isGround);
	void SetWall(bool isWall);
public:
	void SetWallLeft(bool isWallLeft)
	{
		_isWallLeft = isWallLeft;
	}
	void SetWallRight(bool isWallRight)
	{
		_isWallRight = isWallRight;
	}
	void SetCeiling(bool isCeiling)
	{
		_isCeiling = isCeiling;
	}
	bool IsWallLeft() const
	{
		return _isWallLeft;
	}
	bool IsWallRight() const
	{
		return _isWallRight;
	}
	bool IsCeiling() const
	{
		return _isCeiling;
	}
public:
	virtual void DrawInspector() override;
protected:
	float		_gravity = -1200.f;
	float		_maxFallSpeed = -640.f;
	FVector3D	_velocity = FVector3D::Zero;
	bool		_gravityEnabled = true;
	bool		_isJumping = false;
	float		_jumpForce = 1000.f;
	float		_jumpCutMul = 0.4f;
protected:
	float		_fallGravityMul = 1.6f;
	float		_apexGravityMul = 0.5f;
protected:
	float		_apexThreshold = 80.f;
protected:
	bool		_isGrounded		= false;
	bool		_isWallLeft		= false;
	bool		_isWallRight	= false;
	bool		_isCeiling		= false;

};
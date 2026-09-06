#include "pch.h"
#include "PlatformMovementComponent.h"
#include "SceneComponent.h"
#include "../Editor/EditEngine.h"
PlatformMovementComponent::PlatformMovementComponent()
{
}

PlatformMovementComponent::~PlatformMovementComponent()
{
}

void PlatformMovementComponent::Tick(float deltaTime)
{
	ActorComponent::Tick(deltaTime);

	if (!_updateComponent)
	{
		return;
	}

	// 1) 중력 적용 — gravityEnabled이고 공중일 때만
	if (_gravityEnabled && !_isGrounded)
	{
		float gravityMul = 1.f;

		if (_velocity._y < 0)
		{
			gravityMul = _fallGravityMul;
		}
		else if (_velocity._y > 0.f && _isJumping)
		{
			gravityMul = _fallGravityMul;
		}

		if (fabsf(_velocity._y) < _apexThreshold)
		{
			gravityMul *= _apexGravityMul;
		}

		_velocity._y += _gravity * gravityMul * deltaTime;

		if (_velocity._y <= _maxFallSpeed)
		{
			_velocity._y = _maxFallSpeed;
		}
	}
	else if(_isGrounded)
	{
		ZeroVerticalVelocity();
	}

	// 2) 수평: 입력 축 기반(_moveAxis * _speed)
	//    수직: 물리 속도(_velocity._y)
	FVector3D InputVel = _moveAxis * _speed * _speedRange;
	FVector3D delta =  FVector3D(InputVel._x * deltaTime, _velocity._y * deltaTime, 0.f);

	// 매 프레임 grounded 초기화
	// → 충돌 콜백(OnGroundCollision)이 살아있으면 Collision 단계에서 다시 true로 설정됨
	_updateComponent->AddRelativePosition(delta);

	_isGrounded = false;
	_isWallLeft = false;
	_isWallRight = false;
	_isCeiling = false;
}

void PlatformMovementComponent::SetVelocity(const FVector3D& vel)
{
	_velocity = vel;
}

void PlatformMovementComponent::AddVelocity(const FVector3D& vel)
{
	_velocity += vel;
}

void PlatformMovementComponent::AddVelocity(const float& vel)
{
	_velocity._y += vel;
}

void PlatformMovementComponent::ZeroVerticalVelocity()
{
	_velocity._y = 0.f;
}

const FVector3D& PlatformMovementComponent::GetVelocity() const
{
	return _velocity;
}

void PlatformMovementComponent::SetGrounded(bool isGround)
{
	_isGrounded = isGround;
	if (isGround)
	{
		_isJumping = false;
	}
}

bool PlatformMovementComponent::IsGrounded() const
{
	return _isGrounded;
}

void PlatformMovementComponent::SetGravityEnabled(bool enabled)
{
	_gravityEnabled = enabled;
}

void PlatformMovementComponent::SetGravity(float gravity)
{
	_gravity = gravity;
}

void PlatformMovementComponent::SetMaxFallSpeed(float speed)
{
	_maxFallSpeed = speed;
}

void PlatformMovementComponent::StartJump()
{
	_velocity._y = _jumpForce;
	_isJumping = true;
	_isGrounded = false;
}

void PlatformMovementComponent::StartDoubleJump()
{
	_velocity._y = _jumpForce;
	_isJumping = true;
	_isGrounded = false;
}

void PlatformMovementComponent::StopJump()
{
	if (_velocity._y > 0.f)
	{
		_velocity._y *= _jumpCutMul;
	}
	_isJumping = false;
	_speedRange = 1.f;
}

bool PlatformMovementComponent::IsJumping()
{
	return _isJumping;
}

void PlatformMovementComponent::DrawInspector()
{
	Component::DrawInspector();

	ImGui::SeparatorText("PlatformMovementComponent");
	ImGui::DragFloat("Gravity		", &_gravity, 0.1f, -2000.f, 1000.f);
	ImGui::DragFloat("Max Speed		", &_maxFallSpeed, 0.001f, -1000.f, 1000.f);
	ImGui::DragFloat("Jump Power	", &_jumpForce, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("Jump Mul		", &_jumpCutMul, 0.001f, 0.f, 1.f);
}

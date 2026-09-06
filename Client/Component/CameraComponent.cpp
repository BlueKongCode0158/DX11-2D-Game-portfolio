#include "pch.h"
#include "CameraComponent.h"
#include "World/Level.h"
#include "Core/Device.h"

#include "../Object/Actor.h"
#include "../Object/Player.h"

#ifdef _EDITOR
#include "../Editor/EditEngine.h"
#endif // _EDITOR


CameraComponent::CameraComponent()
{
	FResolution rs = Device::Instance().GetRS();
	_width = 1920;
	_height = 1080;
}

CameraComponent::~CameraComponent()
{
}

bool CameraComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	SetProjectionType(_camType);
	Ptr<Level> level = owner->GetLevel();
	if (nullptr == level)
	{
		return false;
	}

	if (!level->GetMainCamera())
	{
		level->SetMainCamera(This<CameraComponent>());
	}
	_type = eComponentType::CAMERA;
	return true;
}

void CameraComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
	_matView.Indentity();

	auto ClampToBound = [&](FVector3D& pos, const  FLocalBound2D& bound)
		{
			const float halfW = _cameraHalf._x * 0.5f;
			const float halfH = _cameraHalf._y * 0.5f;

			float minX = bound._Min._x + halfW;
			float maxX = bound._Max._x - halfW;
			float minY = bound._Min._y + halfH;
			float maxY = bound._Max._y - halfH;

			pos._x = (minX <= maxX) ? Utility::Clamp(pos._x, minX, maxX) : (bound._Min._x + bound._Max._x) * 0.5f;
			pos._y = (minY <= maxY) ? Utility::Clamp(pos._y, minY, maxY) : (bound._Min._y + bound._Max._y) * 0.5f;
		};

	Ptr<class Actor> target = GetTarget();
	if (target)
	{
		FVector3D targetPos = target->GetWorldPosition();

		Ptr<class Player> targetActor = Cast<Actor, Player>(target);
		FVector3D nextPos;

		if (targetActor)
		{
			float desiredOffsetX = targetActor->LookRight() ? _lookAhead : -_lookAhead;
			float offsetLerp = 1.f - expf(-_offsetFollowSpeed * deltaTime);
			_offSet._x = _offSet._x + (desiredOffsetX - _offSet._x) * offsetLerp;
		}

		nextPos = targetPos + _offSet;
		nextPos._z = _world._position._z;
		
		FLocalBound2D desired = _hasLockRequest ? _targetLockBound : _worlLockBound;

		if (desired.IsValid())
		{
			if (!_lockBound.IsValid())
			{
				_lockBound = desired;
			}
			else
			{
				float lockTime = 1.f - expf(-_lockLerpSpeed * deltaTime);
				_lockBound._Min._x += (desired._Min._x - _lockBound._Min._x) * lockTime;
				_lockBound._Min._y += (desired._Min._y - _lockBound._Min._y) * lockTime;
				_lockBound._Max._x += (desired._Max._x - _lockBound._Max._x) * lockTime;
				_lockBound._Max._y += (desired._Max._y - _lockBound._Max._y) * lockTime;
			}
			_isLock = true;
		}

		if (_isLock && _lockBound.IsValid())
		{
			ClampToBound(nextPos, _lockBound);
		}

		if(_worlLockBound.IsValid())
		{
			ClampToBound(nextPos, _worlLockBound);
		}


		if (_snapToTarget)
		{
			_world._position = nextPos;
			_snapToTarget = false;
		}
		else
		{
			float fs = (_trauma > 0.1f) ? _followSpeed * 0.2f : _followSpeed;
			float t = 1.0f - expf(-fs * deltaTime);
			_world._position = Utility::Lerp(_world._position, nextPos, t);
		}
		_relative._position = _world._position;
	}

	_trauma = Utility::Clamp(_trauma - _traumaDecay * deltaTime, 0.f, 1.f);
	_shakeTime += deltaTime;

	FVector3D viewPos = _world._position;
	if (_trauma > 0.f)
	{
		float shake = _trauma * _trauma;	// 비선형(제곱): 큰 충격일수록 강하게
		viewPos._x += _maxShakeOffset * shake * Utility::Noise1D(_shakeTime * _shakeFreq, 0);
		viewPos._y += _maxShakeOffset * shake * Utility::Noise1D(_shakeTime * _shakeFreq, 1);
	}

	for (int i = 0; i < AXIS_TYPE::END; i++)
	{
		FVector3D axis = _axis[i];
		memcpy(&_matView[i][0], &axis, sizeof(FVector3D));
	}

	_matView.Transpose();
	for (int i = 0; i < AXIS_TYPE::END; i++)
	{
		_matView[3][i] = -_axis[i].Dot(viewPos);
	}

	ClearCameraLock();
}

void CameraComponent::Collision(float deltaTime)
{
	SceneComponent::Collision(deltaTime);
}

void CameraComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);
}

void CameraComponent::Destroy()
{
	SceneComponent::Destroy();
}

bool CameraComponent::Save(std::ofstream& file)
{
	return true;
}

bool CameraComponent::Load(std::ifstream& file)
{
	return true;
}

const FLocalBound2D CameraComponent::GetViewBound2D(float margin)
{
	FLocalBound2D bound;

	FVector3D pos = GetWorldPosition();

	float halfW = _width * 0.5f;
	float halfH = _height * 0.5f;

	bound._Min._x = pos._x - halfW - margin;
	bound._Max._x = pos._x + halfW + margin;

	bound._Min._y = pos._y - halfH - margin;
	bound._Max._y = pos._y + halfH + margin;

	return bound;
}

// 해상도 바뀔때마다 호출해줘야함.
void CameraComponent::SetProjectionType(eCameraType type)
{
	_camType = type;

	switch (_camType)
	{
	case eCameraType::Perspective:
		_matProj = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(120.f),   // 수직 FOV (radian)
			_width / _height,       // aspect ratio
			0.5,                 // near clip (0보다 커야 함! 0.3 권장)
			_viewDistance           // far clip
		);
		break;	
	case eCameraType::Ortho:
		_matProj = DirectX::XMMatrixOrthographicOffCenterLH(_width / -2, _width / 2, _height / -2, _height / 2, 0.f, _viewDistance);
		break;
	default:
		break;
	}
}

void CameraComponent::SetWorldLockBound(FLocalBound2D& worldLock)
{
	_worlLockBound = worldLock;
}

void CameraComponent::SetCameraLock(FLocalBound2D& lockBound)
{
	_targetLockBound = lockBound;
	_hasLockRequest = true;
}

void CameraComponent::SetCameraHalf(FVector2D& cameraHalf)
{
	_cameraHalf._x = cameraHalf._x * 64.f;
	_cameraHalf._y = cameraHalf._y * 64.f;
}

void CameraComponent::ClearCameraLock()
{
	_hasLockRequest = false;
}

void CameraComponent::SetCameraTarget(Ptr<class Actor> target)
{
	_target = target;
	_snapToTarget = true;
	_trauma = 0.f;
	_lockBound = FLocalBound2D{};
	_isLock = false;
	_hasLockRequest = false;
	_offSet = FVector3D(0.f, 200.f, 0.f);

	FVector3D targetPos = target->GetRelativePosition();

	_world._position._x = targetPos._x;
	_world._position._y = targetPos._y;
	_world._position._z = -100.f;
	_relative._position = _world._position;

	UpdateTransform();
}

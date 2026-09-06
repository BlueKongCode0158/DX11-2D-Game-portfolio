#pragma once
#include "SceneComponent.h"

enum eCameraType
{
	Perspective,
	Ortho
};

class CameraComponent : public SceneComponent
{
public:
	CameraComponent();
	virtual ~CameraComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	const FMatrix& GetViewMatrix() const		{ return _matView; }
	const FMatrix& GetProjectionMatrix() const	{ return _matProj; }
	const FLocalBound2D GetViewBound2D(float margin);
	// get
	eCameraType GetProjectionType() const	{ return _camType; }
	float GetWidth() const					{ return _width; }
	float GetHight() const					{ return _height; }
	float GetViewAngle() const				{ return _viewAngle; }
	float GetViewDistance() const			{ return _viewDistance; }

	// set
	void SetProjectionType(eCameraType type);
	void SetWidth(float width) 				 { _width = width;	}
	void SetHight(float height) 			 { _height = height; }
	void SetViewAngle(float angle) 			 { _viewAngle = angle; }
	void SetViewDistance(float dist) 		 { _viewDistance = dist; }
public:
	void AddTrauma(float trauma)			 { _trauma += trauma; }
public:
	void SetWorldLockBound(FLocalBound2D& worldLock);
public:
	Ptr<class Actor> GetTarget()
	{
		return Lock<Actor>(_target);
	}
public:
	void SetCameraLock(FLocalBound2D& lockBound);
	void SetCameraHalf(FVector2D& cameraHalf);
	void ClearCameraLock();
public:
	void SetCameraTarget(Ptr<class Actor> target);
protected:
	FMatrix _matView = {};
	FMatrix _matProj = {};
	eCameraType _camType = eCameraType::Ortho;
	float _viewAngle = 90.f;
	float _viewDistance = 5000.f;
	float _width = 0.f;
	float _height = 0.f;
protected:
	Weak<class Actor> _target;
	float _followSpeed = 5.f;
	FVector3D _offSet = FVector3D(0.f, 200.f, 0.f);
protected:
	float _lookAhead = 100.f;
	float _offsetFollowSpeed = 3.f;
protected:
	FVector2D _cameraHalf; 
protected:
	FLocalBound2D _worlLockBound;
	FLocalBound2D _targetLockBound;
	float	_lockLerpSpeed = 1.2f;
	bool	_hasLockRequest = false;
protected:
	FLocalBound2D	_lockBound;
	bool			_isLock = false;
protected:
	float _trauma			= 0.f;		// 0~1, 외상 누적값
	float _traumaDecay		= 1.2f;		// 초당 감쇠량 (클수록 빨리 멈춤)
	float _shakeTime		= 0.f;		// 노이즈 진행 시간 누적
	float _maxShakeOffset	= 60.f;	// 최대 흔들림 (월드 단위 / 픽셀 단위)
	float _shakeFreq		= 25.f;		// 흔들림 빠르기
protected:
	bool _snapToTarget = false;
};
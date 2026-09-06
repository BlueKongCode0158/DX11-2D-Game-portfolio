#pragma once
#include "Object/Object.h"

class CameraManager : public Object
{
public:
	CameraManager() = default;
	virtual ~CameraManager() = default;
	CameraManager(const CameraManager&) = delete;
	CameraManager(CameraManager&&) = delete;
	CameraManager& operator =(const CameraManager&) = delete;
	CameraManager& operator =(CameraManager&&) = delete;
public:
	bool Init(Ptr<class Level> level);
	void Tick(float deltaTime);
	void Render(float deltaTime);
	void SetMainCamera(Ptr<class CameraComponent> camera);
	Ptr<class CameraComponent> GetMainCamera() const;
	const FMatrix& GetWorldMatrix() const;
	const FMatrix& GetViewMatrix() const;
	const FMatrix& GetProjMatrix() const;
	const FMatrix& GetUIProjMatrix() const;
	const FVector3D& GetCameraWorldPos() const;
	const FLocalBound2D GetViewBound2D(float margin);
public:
	void SetCameraTarget(Ptr<class Actor> target);
	void SetCurLevel(Ptr<class Level> level);
public:
	virtual void Destroy() override;
	void CreateUIProj(float x, float y, float dist);
private:
	Ptr<class CameraComponent> _mainCamera;
	// todo : 카메라 목록 -> 카메라 연출
	std::unordered_map<std::string, Ptr<class CameraComponent>> _cameras;
	FMatrix _uiProj;
	Weak<class Level> _level;
};
#include "pch.h"
#include "CameraManager.h"
#include "Component/CameraComponent.h"

#include "../Core/Device.h"

bool CameraManager::Init(Ptr<class Level> level)
{
	_level = level;

	FResolution rs/* = Device::Instance().GetRS()*/;

	rs._width = 1920;
	rs._height = 1080;
	CreateUIProj(static_cast<float>(rs._width), static_cast<float>(rs._height), 2000);
	
	return true;
}

void CameraManager::Tick(float deltaTime)
{
	_mainCamera->Tick(deltaTime);
}

void CameraManager::Render(float deltaTime)
{
}

void CameraManager::SetMainCamera(Ptr<class CameraComponent> camera)
{
	_mainCamera = camera;
}

Ptr<class CameraComponent> CameraManager::GetMainCamera() const
{
	return _mainCamera;
}

const FMatrix& CameraManager::GetWorldMatrix() const
{
	return _mainCamera->GetWorldMatrix();
}

const FMatrix& CameraManager::GetViewMatrix() const
{
	return _mainCamera->GetViewMatrix();
}

const FMatrix& CameraManager::GetProjMatrix() const
{
	return _mainCamera->GetProjectionMatrix();
}

const FMatrix& CameraManager::GetUIProjMatrix() const
{
	return _uiProj;
}

const FVector3D& CameraManager::GetCameraWorldPos() const
{
	return _mainCamera->GetWorldPosition();
}

const FLocalBound2D CameraManager::GetViewBound2D(float margin)
{
	return _mainCamera->GetViewBound2D(margin);
}

void CameraManager::SetCameraTarget(Ptr<class Actor> target)
{
	_mainCamera->SetCameraTarget(target);
}

void CameraManager::SetCurLevel(Ptr<class Level> level)
{
	_level = level;
}

void CameraManager::Destroy()
{
	// TODO CameraManager Destroy
}

void CameraManager::CreateUIProj(float x, float y, float dist)
{
	_uiProj = DirectX::XMMatrixOrthographicOffCenterLH(0.f, x, 0.f, y, 0, dist);
}

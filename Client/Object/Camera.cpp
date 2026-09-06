#include "pch.h"
#include "Camera.h"

#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"

#include "../World/Level.h"

bool Camera::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);

	_camera = CreateSceneComponent<CameraComponent>("mainCamera");
	SetRootComponent(_camera);
	return true;
}

void Camera::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Camera::ChanageLevel(Ptr<class Level> level)
{
	level->SetMainCamera(_camera);
}

void Camera::Destroy()
{
	Actor::Destroy();
}

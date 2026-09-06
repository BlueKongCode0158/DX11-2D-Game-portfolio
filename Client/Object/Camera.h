#pragma once
#include "Actor.h"

class Camera : public Actor
{
public:
	Camera() = default;
	virtual ~Camera() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual	void Tick(float deltaTime);
public:
	void ChanageLevel(Ptr<class Level> level);
public:
	virtual void Destroy() override;
private:
	Ptr<class CameraComponent> _camera;
	Ptr<class InputCompontnt> _input;
};
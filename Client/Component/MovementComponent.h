#pragma once
#include "Component/ActorComponent.h"

class MovementComponent : public ActorComponent
{
public :
	MovementComponent() = default;
	virtual ~MovementComponent() = default;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime);
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetSpeed(float speed) { _speed = speed; }
	const float GetSpeed() const { return _speed; }
	void SetUpdateComponent(Ptr<class SceneComponent> updateComp);
	FVector3D GetMoveAxis() const { return _moveAxis; }
	Ptr<class SceneComponent> GetUpdateComponent() const;
	void SetMoveAxis(const FVector3D& moveAxis);
	void AddMoveAxis(const FVector3D& moveAxis);
	void SetSpeedRange(float range) { _speedRange = range; }
	void Stop();
protected:
	float _speed = 0.f;
	float _speedRange = 1.f;
	FVector3D _moveAxis = FVector3D::Zero;
	FVector3D _nextPosition = FVector3D::Zero;
	Ptr<class SceneComponent> _updateComponent = nullptr;
};
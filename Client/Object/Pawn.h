#pragma once
#include "Object/Actor.h"
#include "../Editor/Interface/Interface.h"

class Pawn : public Actor
{
public:
	Pawn();
	virtual ~Pawn();
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
public:
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file) override;
	virtual bool Load(std::ifstream& file) override;
protected:
	void SetController(Ptr<class Controller> ctrl);
protected:
	Ptr<Controller> _controller;
public:
	template<typename T>
	Ptr<T> GetController() const
	{
		return Cast<class Controller, T>(_controller);
	}
};
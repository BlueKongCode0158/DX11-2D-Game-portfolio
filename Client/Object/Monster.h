#pragma once
#include "Object/Pawn.h"

class Monster : public Pawn
{
public:
	Monster() = default;
	virtual ~Monster() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetTarget(Ptr<class Player> player);
	void BlockCallback(Weak<class CollisionComponent> comp);
private:
	int32 _timerID = -1;
	Weak<class Player>  _target;
	Ptr<class AABBCollisionComponent> _col;
};
#pragma once
#include "Actor.h"

// pivot 만들면서 필요없어진 것 같기도

enum PLAYERSLASHTYPE
{
	ATTACK,
	DOUBLE_ATTACK,
	UP_ATTACK,
	DOWN_ATTACK
};
class PlayerSlash : public Actor
{
public:
	PlayerSlash() = default;
	virtual ~PlayerSlash() = default;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Collision(float deltaTime);
public:
	virtual void Destroy() override;
public:
	void SetAttack(PLAYERSLASHTYPE eAttackType);
	void SetDir(bool isRight);
private:
	Ptr<class SpriteComponent> _meshComp;
	Ptr<class AABBCollisionComponent> _collisionComp;
};

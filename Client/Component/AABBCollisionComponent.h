#pragma once
#include "CollisionComponent.h"

class AABBCollisionComponent : public CollisionComponent
{
public:
	AABBCollisionComponent() = default;
	virtual ~AABBCollisionComponent() = default;
public:
	FLocalBound2D GetLocalBound();
	const FVector2D& GetBoxSize() const { return _boxSize; }
	const FAABB2D& GetBox() const { return _box; }
	void SetBoxSize(const FVector2D& boxSize)
	{
		SetBoxSize(boxSize._x, boxSize._y);
	}
	void SetBoxSize(float x, float y)
	{
		_boxSize._x = x;
		_boxSize._y = y;
	}
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Collision(Weak<CollisionComponent> dest) override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
private:
	FVector2D	_boxSize;
	FAABB2D		_box;
};
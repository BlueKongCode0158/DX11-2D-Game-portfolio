#pragma once
#include "CollisionComponent.h"

class OBBCollisionComponent : CollisionComponent
{
public:
	OBBCollisionComponent() = default;
	virtual ~OBBCollisionComponent() = default;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	virtual bool Collision(Weak<CollisionComponent> dest);
public:
	const FOBB2D& GetBox() const { return _box; }
	const FVector2D& GetBoxSize() const { return _boxSize; }
	void SetBoxSize(const FVector2D& size) { SetBoxSize(size._x, size._y); }
	void SetBoxSize(const float x, const float y) { _boxSize._x = x; _boxSize._y = y; }
protected:
	FVector2D _boxSize;
	FOBB2D _box;
};
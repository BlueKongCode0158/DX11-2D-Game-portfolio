#pragma once
#include "CollisionComponent.h"

class EdgeCollisionComponent : public CollisionComponent
{
public:
	EdgeCollisionComponent();
	virtual ~EdgeCollisionComponent();
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
	const FEDGE2D& GetLine() const { return _Line; }
	void SetLine(const FVector2D& startPoint, const FVector2D& endPoint);
	void SetLine(FEDGE2D& line);
private:
	void ReflashLinePoint();
private:
	FEDGE2D _Line;
};

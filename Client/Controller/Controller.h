#pragma once
#include "Object/Actor.h"

class Controller : public Actor
{
public:
	Controller() = default;
	virtual ~Controller() = default;
	Controller(const Controller&) = delete;
	Controller(Controller&&) = delete;
	Controller& operator = (const Controller&) = delete;
	Controller& operator = (Controller&&) = delete;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
public:
	virtual void Destroy() override;
	Ptr<class Pawn> GetPawn() const;
	void SetPawn(Ptr<class Pawn> pawn);
private:
	Weak<class Pawn> _pawn;
};

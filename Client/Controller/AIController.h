#pragma once
#include "Controller.h"

class AIController : public Controller
{
public:
	AIController();
	virtual ~AIController();
	AIController(const AIController&) = delete;
	AIController(AIController&&) = delete;
	AIController& operator = (const AIController&) = delete;
	AIController& operator = (AIController&&) = delete;
protected:
	Ptr<class AIComponent> _aiComponent;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
public:
	Ptr<class AIComponent> GetAI() const;
	Ptr<class MachineBase> GetAIStateMachine() const;
public:
	virtual void Destroy() override;
};
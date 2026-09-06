#pragma once
#include "Controller/AIController.h"
#include "Component/InputComponent.h"

class PlayerController : public AIController
{
public:
	PlayerController() = default;
	virtual ~PlayerController() = default;
	PlayerController(const PlayerController&) = delete;
	PlayerController(PlayerController&&) = delete;
	PlayerController& operator = (const PlayerController&) = delete;
	PlayerController& operator = (PlayerController&&) = delete;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name) override;
	virtual void Tick(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
public:
	virtual void Destroy() override;
	Ptr<InputComponent> GetInputComponent() const;
private:
	Ptr<InputComponent>_input;
};
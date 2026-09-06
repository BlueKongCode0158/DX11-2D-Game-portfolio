#pragma once
#include "MovementComponent.h"

class TileRideMovementComponent : public MovementComponent
{
public:
	TileRideMovementComponent();
	virtual ~TileRideMovementComponent();
public:
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
};

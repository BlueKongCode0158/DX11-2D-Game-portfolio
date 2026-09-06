#pragma once
#include "MachineBase.h"

class BuzzerStateMachine : public MachineBase
{
public:
	BuzzerStateMachine() {};
	virtual ~BuzzerStateMachine() {};
public:
	virtual void Init(Ptr<class AIComponent> owner) override;
	virtual void Destroy() override;
private:
	Ptr<class PlayerTargetMonsterBoard> GetBoard();
	Ptr<class Player> GetTarget();
	Ptr<class Buzzer> GetBuzzer();

};
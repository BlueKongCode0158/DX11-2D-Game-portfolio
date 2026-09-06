#include "pch.h"
#include "BuzzerStateMachine.h"
#include "AIBoard.h"

#include "../../Object/Buzzer.h"
#include "../../Object/Player.h"

void BuzzerStateMachine::Init(Ptr<class AIComponent> owner)
{
    MachineBase::Init(owner);
    _interval = 0.1f;
    auto board = CreateAIBoard<PlayerTargetMonsterBoard>();

}

void BuzzerStateMachine::Destroy()
{
    MachineBase::Destroy();
}

Ptr<class PlayerTargetMonsterBoard> BuzzerStateMachine::GetBoard()
{
    return Cast<AIBoard, PlayerTargetMonsterBoard>(_board);
}

Ptr<class Player> BuzzerStateMachine::GetTarget()
{
	auto board = GetBoard();
	if (!board)
	{
		return nullptr;
	}
	return Cast<Actor, Player>(Lock<Actor>(board->_target));
}

Ptr<class Buzzer> BuzzerStateMachine::GetBuzzer()
{
	auto board = GetBoard();
	if (!board)
	{
		return nullptr;
	}
	return Cast<Actor, Buzzer>(Lock<Actor>(board->_actor));
}

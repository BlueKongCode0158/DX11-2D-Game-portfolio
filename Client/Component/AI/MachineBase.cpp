#include "pch.h"
#include "AIState.h"
#include "AITransition.h"
#include "AIBoard.h"
#include "MachineBase.h"

MachineBase::MachineBase()
{
}

MachineBase::~MachineBase()
{
}

void MachineBase::Init(Ptr<class AIComponent> owner)
{
	_owner = owner;
}

void MachineBase::Tick(float deltaTime)
{
	if (!_curState)
	{
		return;
	}

	_curInterval += deltaTime;
	if (_curInterval < _interval)
	{
		return;
	}

	_curState->OnTick(deltaTime);

	Ptr<AIState> nextState = _curState->Tick(deltaTime);
	if (nextState && nextState != _curState)
	{
		_curState->OnExit(deltaTime);
		nextState->OnEnter(deltaTime);
		_curState = nextState;

		OnChangedState();
	}
	_curInterval -= _interval;
}

void MachineBase::Destroy()
{
	DESTROY(_board);
	for (auto& it : _stateFinder)
	{
		DESTROY(it.second);
	}
	_stateFinder.clear();
}

Ptr<class AIComponent> MachineBase::GetAiComponent() const
{
	return Lock<AIComponent>(_owner);
}

void MachineBase::OnChangedState()
{
}

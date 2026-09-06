#include "pch.h"
#include "AITransition.h"

AITransition::AITransition()
{
}

AITransition::~AITransition()
{
}

bool AITransition::CheckCondition(float deltaTime)
{
	if (_conditions.size() < 1)
	{
		return false;
	}

	switch (_rule)
	{
	case eTransitionRule::AND:
		for (auto& it : _conditions)
		{
			if (!it())
			{
				return false;
			}
		}
		return true;
	case eTransitionRule::OR:
		for (auto& it : _conditions)
		{
			if (it())
			{
				return true;
			}
		}
		return false;
	}

	return true;
}

bool AITransition::Init(const std::string& name, Ptr<AIState> jumpState, eTransitionRule rule)
{
	_name = name;
	_jumpState = jumpState;
	_rule = rule;

	return true;
}

void AITransition::Destroy()
{

}

void AITransition::SetName(const std::string& name)
{
	_name = name;
}

const std::string& AITransition::GetName()
{
	return _name;
}

Ptr<class AIComponent> AITransition::GetAIComponent() const
{
	return Lock<AIComponent>(_aiComponent);
}

void AITransition::SetAIComponent(Ptr<class AIComponent> comp)
{
	_aiComponent = comp;
}

Ptr<class AIState> AITransition::GetJumpState() const
{
	return _jumpState;
}

void AITransition::SetJumpState(Ptr<class AIState> state)
{
	_jumpState = state;
}

void AITransition::SetTransitionRule(eTransitionRule rule)
{
	_rule = rule;
}

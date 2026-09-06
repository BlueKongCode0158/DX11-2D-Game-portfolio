#pragma once
#include "../../Object/Object.h"
#include "AIState.h"

class MachineBase : public Object
{
	friend class AIComponent;
public:
	MachineBase();
	virtual ~MachineBase();
protected:
	Weak<class AIComponent> _owner;
	Ptr<class AIBoard> _board;
	Ptr<AIState> _curState;
	std::unordered_map<std::string, Ptr<AIState>> _stateFinder;

	float _curInterval = 0.f;
	float _interval = 0.5f;
public:
	virtual void Init(Ptr<class AIComponent> owner);
	virtual void Tick(float deltaTime);
	virtual void Destroy() override;
	Ptr<class AIComponent> GetAiComponent() const;
protected:
	virtual void OnChangedState();
public:
	template<typename T>
	Ptr<T> GetAIBoard()
	{
		return Cast<AIBoard, T>(_board);
	}

	template<typename T>
	Ptr<T> FindAIState(const std::string& name)
	{
		auto it = _stateFinder.find(name);
		if (_stateFinder.end() == it)
		{
			return nullptr;
		}
		return it->second;
	}

	template<typename T>
	Ptr<T> CreateAIState(const std::string& name)
	{
		auto state = FindAIState<T>(name);
		if (nullptr != state)
		{
			return state;
		}

		state = New<T>();
		if (false == state->Init(name))
		{
			DESTROY(state);
			return nullptr;
		}

		if (!_curState)
		{
			_curState = state;
		}

		_stateFinder[name] = state;
		return state;
	}

	template<typename T>
	Ptr<T> CreateAITransition(const std::string& stateName, const std::string& tranName, const std::string& jumpStateName, eTransitionRule rule)
	{
		auto state = FindAIState<AIState>(stateName);
		if (nullptr == state)
		{
			return nullptr;
		}

		auto jumpState = FindAIState<AIState>(jumpStateName);

		return state->CreateAITransition<T>(tranName, jumpState, rule);
	}

	template<typename T>
	Ptr<T> CreateAIBoard()
	{
		if (_board)
		{
			return Cast<AIBoard, T>(_board);
		}
		_board = New<T>();
		return Cast<AIBoard, T>(_board);
	}
};
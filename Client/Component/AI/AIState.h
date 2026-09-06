#pragma once
#include "../../Object/Object.h"
#include <functional>

class AIState : public Object
{
	friend class AIComponent;
public:
	AIState();
	virtual ~AIState();
protected:
	std::string _name;
	std::function<void(float)> _callback[AI_EVENT_STATE::END];
	std::vector<Ptr<class AITransition>> _transitions;
public:
	virtual bool Init(const std::string& name);
	virtual Ptr<AIState> Tick(float deltaTime);
public:
	void SetName(const std::string& name);
	const std::string& GetName() const;	
	virtual void OnExit(float deltaTime);
	virtual void OnEnter(float deltaTime);
	virtual void OnTick(float deltaTime);
public:
	virtual void Destroy() override;
public:
	template<typename T>
	Ptr<T> CreateAITransition(const std::string& name, Ptr<AIState> jumpState, eTransitionRule rule)
	{
		auto tran = FindAITransition<T>(name);
		if (tran)
		{
			return tran;
		}

		tran = New<T>();
		if (false == tran->Init(name, jumpState, rule))
		{
			DESTROY(tran);
			return nullptr;
		}

		_transitions.push_back(tran);
		return tran;
	}

	template<typename T>
	void AddExitCallback(T&& func)
	{
		_callback[AI_EVENT_STATE::EXIT] = std::forward<T>(func);
	}

	template<typename T>
	void AddExitCallback(T* obj, void(T::* memFunc)(float))
	{
		_callback[AI_EVENT_STATE::EXIT] = std::bind(memFunc, obj, std::placeholders::_1);
	}
	template<typename T>
	void AddEnterCallback(T&& func)
	{
		_callback[AI_EVENT_STATE::ENTER] = std::forward<T>(func);
	}

	template<typename T>
	void AddEnterCallback(T* obj, void(T::* memFunc)(float))
	{
		_callback[AI_EVENT_STATE::ENTER] = std::bind(memFunc, obj, std::placeholders::_1);
	}
	template<typename T>
	void AddTickCallback(T&& func)
	{
		_callback[AI_EVENT_STATE::TICK] = std::forward<T>(func);
	}

	template<typename T>
	void AddTickCallback(T* obj, void(T::* memFunc)(float))
	{
		_callback[AI_EVENT_STATE::TICK] = std::bind(memFunc, obj, std::placeholders::_1);
	}

	template<typename T>
	void AddCondition(const std::string& name, T&& func)
	{
		for (auto& it : _transitions)
		{
			if (it->GetName() == name)
			{
				it->AddCondition(func);
				break;
			}
		}
	}

	template<typename T>
	void AddCondition(const std::string& name, T* obj, bool (T::*memFunc)())
	{
		for (auto& it : _transitions)
		{
			if (it->GetName() == name)
			{
				it->AddCondition(obj, memFunc);
				break;
			}
		}
	}

	template<typename T>
	Ptr<T> FindAITransition(const std::string& name)
	{
		for (auto& it : _transitions)
		{
			if (it->GetName() == name)
			{
				return it;
			}
		}
		return nullptr;
	}

};
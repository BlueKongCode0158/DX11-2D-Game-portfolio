#pragma once
#include "../../Object/Object.h"
#include <functional>


class AITransition : public Object
{
	friend class AIState;
	friend class AIComponent;
public:
	AITransition();
	virtual ~AITransition();
protected:
	std::string _name;
	Weak<class AIComponent> _aiComponent;

	// 조건을 어떤 방식으로 충족할지
	eTransitionRule _rule = eTransitionRule::OR;

	// 다른 상태로 넘어가기 위한 조건들을 체크
	std::vector<std::function<bool()>> _conditions;

	// _conditons가 충족한다면 전환될 상태
	Ptr<class AIState> _jumpState;

protected:
	virtual bool CheckCondition(float deltaTime);
public:
	bool Init(const std::string& name, Ptr<AIState> jumpState, eTransitionRule rule);
	virtual void Destroy() override;
public:
	void SetName(const std::string& name);
	const std::string& GetName();
	Ptr<class AIComponent> GetAIComponent() const;
	void SetAIComponent(Ptr<class AIComponent> comp);
	Ptr<class AIState> GetJumpState() const;
	void SetJumpState(Ptr<class AIState> state);
	void SetTransitionRule(eTransitionRule rule);
public:
	template<typename T>
	void AddCondition(T&& func)
	{
		_conditions.push_back(std::forward<T>(func));
	}

	template<typename T>
	void AddCondition(T* obj, bool (T::* memFunc)())
	{
		_conditions.push_back(std::bind(memFunc, obj));
	}
};
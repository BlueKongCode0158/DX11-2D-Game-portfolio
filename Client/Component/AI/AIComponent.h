#pragma once
#include "MachineBase.h"
#include "../ActorComponent.h"

class AIComponent : public ActorComponent
{
public:
	AIComponent();
	virtual ~AIComponent();
protected:
	Weak<class AIController> _controller;
	Ptr<class MachineBase> _machine;
	//Ptr<class AIBoard> _board;
	//Ptr<class AIState> _curState;
	//Ptr<class MachinBase> _machine;
	//std::unordered_map<std::string, Ptr<class AIState>> _stateFinder;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetController(Ptr<class AIController> controller);
	Ptr<class AIController> GetController() const;
public:
	//Ptr<AIState> FindState(const std::string& name);
	Ptr<class MachineBase> GetAIStateMachine();
public:
	template<typename T>
	void CreateAIStateMachine()
	{
		if (_machine)
		{
			return;
		}

		_machine = New<T>();
		_machine->Init(This<AIComponent>());
	}
};
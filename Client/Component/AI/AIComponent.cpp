#include "pch.h"
#include "AIComponent.h"
#include "../../Controller/AIController.h"

AIComponent::AIComponent()
{
}

AIComponent::~AIComponent()
{
}

bool AIComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	ActorComponent::Init(id, name, owner);
	_type = eComponentType::AI;
	return true;
}

void AIComponent::Tick(float deltaTime)
{
	ActorComponent::Tick(deltaTime);

	if (!_machine)
	{
		return;
	}

	_machine->Tick(deltaTime);
}

void AIComponent::Destroy()
{
	ActorComponent::Destroy();
	DESTROY(_machine);
	//for (auto& it : _stateFinder)
	//{
	//	DESTROY(it.second);
	//}
	//_stateFinder.clear();
}

bool AIComponent::Save(std::ofstream& file)
{
	return true;
}

bool AIComponent::Load(std::ifstream& file)
{
	return true;
}

void AIComponent::SetController(Ptr<class AIController> controller)
{
	_controller = controller;
}

Ptr<class AIController> AIComponent::GetController() const
{
	return Lock<AIController>(_controller);
}

Ptr<class MachineBase> AIComponent::GetAIStateMachine()
{
	return _machine;
}

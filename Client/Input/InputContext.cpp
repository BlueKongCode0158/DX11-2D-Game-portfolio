#include "pch.h"
#include "InputContext.h"
#include "InputSystem.h"

const std::string& InputContext::GetName() const
{
	return _name;
}

void InputContext::SetName(const std::string& name)
{
	_name = name;
}

void InputContext::BindInputAction(Ptr<InputAction> action, uint8 key)
{
	// TODO : Bind 선생님 코드 
	auto it = _mappings.find(action->GetName());
	if (_mappings.end() != it)
	{
		return;
	}

	FInputMapping mapping;
	mapping._action = action;
	mapping._key = InputSystem::Instance().ConvertKey(key);
	_mappings[action->GetName()] = mapping;
	InputSystem::Instance().RefreshInputComponent(_name, _mappings[action->GetName()]);
}

void InputContext::ChangeInputActionKey(const std::string& actionName, uint8 key)
{
	auto it = _mappings.find(actionName);
	if (_mappings.end() != it)
	{
		_mappings[actionName]._key = InputSystem::Instance().ConvertKey(key);
	}
}

FInputMapping* InputContext::FindMapping(const std::string& name)
{
	auto it = _mappings.find(name);
	if (_mappings.end() == it)
	{
		return nullptr;
	}

	return &(it->second);
}

void InputContext::Destroy()
{
	// 안에 있는게 포인터가 아니라 값이니까 그냥 clear한다.
	_mappings.clear();
}

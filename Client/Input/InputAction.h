#pragma once
#include "Object/Object.h"

class InputAction : public Object
{
	friend class InputSystem;
public:
	InputAction() = default;
	virtual ~InputAction() = default;
	InputAction(const InputAction&) = delete;
	InputAction(InputAction&&) = delete;
	InputAction& operator = (const InputAction&) = delete;
	InputAction& operator = (InputAction&&) = delete;
private:
	std::string _name;
public:
	void SetName(const std::string name) { _name = name; }
	std::string GetName() const { return _name; }
	virtual void Destroy() override;
};
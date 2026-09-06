#pragma once
#include "Object/Object.h"
#include "InputAction.h"

// 
struct FInputMapping
{
	Ptr<InputAction> _action;
	bool _state[INPUT_TYPE::END] = {};	// 입력 상태 [ 누름 / 누르고 있음 / 뗐음 ]
	uint8 _key = 0;							// 실제 하드웨어키 (마우스 / 키보드)
};

class InputContext : public Object
{
	friend class InputSystem;
	friend class InputComponent;
public:
	InputContext() = default;
	virtual ~InputContext() = default;
public:
	InputContext(const InputContext&) = delete;
	InputContext(InputContext&&) = delete;
	InputContext& operator = (const InputContext&) = delete;
	InputContext& operator = (InputContext&&) = delete;
public:
	const std::string& GetName() const;
	void SetName(const std::string& name);
	void BindInputAction(Ptr<InputAction> action, uint8 key);
	void ChangeInputActionKey(const std::string& actionName, uint8 key);
	FInputMapping* FindMapping(const std::string & name);
	virtual void Destroy() override;
private:
	std::string _name;
	std::unordered_map<std::string, FInputMapping> _mappings;
};
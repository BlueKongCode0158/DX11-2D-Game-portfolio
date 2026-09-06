#pragma once
#include "ActorComponent.h"
#include <functional>

struct FBindAction
{
	std::string _name;
	struct FInputMapping* _mapping = nullptr;
	std::function<void(float)> _callBack[INPUT_TYPE::END];
};

class InputComponent : public ActorComponent
{
public:
	InputComponent() = default;
	virtual ~InputComponent() = default;
private:
	std::unordered_map<std::string, std::unordered_map<std::string, FBindAction>> _binds;
	std::string _activeContext;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Destroy();
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetActiveContext(const std::string& name);
	const std::string& GetActiveContext(const std::string& name);
public:
	void AddInputContext(const std::string& name);
	void RemoveInputContext(const std::string& name);
	void RefreshInputMapping(const std::string& contextName, const FInputMapping& mapping);

	// 람다 혹은 함수받기
	template<typename T>
	void BindAction(const std::string& contextName, const std::string& actionName, INPUT_TYPE::eType type, T&& func)
	{
		auto it = _binds.find(contextName);
		if (_binds.end() == it)
		{
			return;
		}

		_binds[contextName][actionName]._callBack[type] = std::forward<T>(func);
	}

	template<typename T>
	void BindAction(const std::string& contextName, const std::string& actionName, INPUT_TYPE::eType type, T* obj, void(T::* memFunc)(float))
	{
		auto it = _binds.find(contextName);
		if (_binds.end() == it)
		{
			return;
		}

		_binds[contextName][actionName]._callBack[type] = std::bind(memFunc, obj, std::placeholders::_1);
	}
};
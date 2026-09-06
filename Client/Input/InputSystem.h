#pragma once
#include "Common/Singleton.h"

class InputSystem : public Singleton<InputSystem>
{
	DECLARE_SINGLETON(InputSystem);
public:
	bool Init(Ptr<class Input> input);
	void Tick(float deltaTime);
public:
	void AddActiveInputContext(Ptr<class InputContext> context);
	void RemoveActiveContext(const std::string& name);
	Ptr<class InputContext> FindOrAddInputContext(const std::string& name);
	Ptr<class InputAction> FindOrAddInputAction(const std::string& name);
	bool IsActiveContext(const std::string& name);
	uint8 ConvertKey(uint8 key);
	// Singleton을(를) 통해 상속됨
	void Destroy() override;
public:
	void RegisterInputComponent(int32 actorID, Ptr<class InputComponent> inputComp);
	void RemoveInputComponent(int32 actorID, int32 componentID);
	void RefreshInputComponent(const std::string& contextName, const struct FInputMapping& mapping);
public:
	const FVector2D& GetMouseWorldPos() const;
	const FVector2D& GetMousePos() const;
	const FVector2D& GetMouseMove() const;
private:
	std::unordered_map<std::string, Ptr<class InputContext>> _activeContexts;
	std::unordered_map<std::string, Ptr<class InputContext>> _allContexts;
	std::unordered_map<std::string, Ptr<class InputAction>> _allActions;
	std::map<std::pair<int32, int32>, Weak<class InputComponent>> _inputComponents;
	Ptr<class Input> _input;
}; 
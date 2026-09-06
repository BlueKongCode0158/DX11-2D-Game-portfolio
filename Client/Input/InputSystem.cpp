#include "pch.h"
#include "InputSystem.h"
#include "InputContext.h"
#include "InputAction.h"
#include "Input.h"
#include "Component/InputComponent.h"

bool InputSystem::Init(Ptr<class Input> input)
{
    _input = input;

    // WANT : 미리 컨텍스트나 액션을 만들어도 된다.
    return true;
}

void InputSystem::Tick(float deltaTime)
{
    for (auto& it : _activeContexts)
    {
        for (auto& subIt : it.second->_mappings)
        {
            FInputMapping& mapping = subIt.second;
            bool KeyPush = _input->GetKeyState(mapping._key);
            
            // 키를 눌렀을 때
            if (KeyPush)
            {
                // 기존 키가 아무것도 눌려있지 않은 상태라면
                // 첫 입력도 들어가지 않았고, 계속 누르고 있는 상태도 아니야.
                if (!mapping._state[INPUT_TYPE::DOWN] && !mapping._state[INPUT_TYPE::HOLD])
                {
                    mapping._state[INPUT_TYPE::DOWN] = true;
                    mapping._state[INPUT_TYPE::HOLD] = true;
                }
                // 기존키가 계속 눌려있어. -> 그러니까 맨 처음 입력은 사라져.
                else
                {
                    mapping._state[INPUT_TYPE::DOWN] = false;
                }
            }
            // 키를 누르지 않았어. -> 뗐어.
            else
            {
                // 근데 만약 기존키가 계속 눌려있을 상태야 -> 그럼 해당 키들을 싹 다 꺼줘야겠지.
                // 근데 손을 뗐다는 뜻이니까 up은 true가 돼.
                if (mapping._state[INPUT_TYPE::HOLD])
                {
                    mapping._state[INPUT_TYPE::DOWN] = false;
                    mapping._state[INPUT_TYPE::HOLD] = false;
                    mapping._state[INPUT_TYPE::UP] = true;
                } // 키를 누르고 있지 않는 상태야.
                else
                {
                    mapping._state[INPUT_TYPE::UP] = false;
                }
            }
        }
    }
}

// 활동 컨텍스트를 모아놓는 상자
void InputSystem::AddActiveInputContext(Ptr<class InputContext> context)
{
    _activeContexts[context->_name] = context;
}

void InputSystem::RemoveActiveContext(const std::string& name)
{
    _activeContexts.erase(name);
}

Ptr<class InputContext> InputSystem::FindOrAddInputContext(const std::string& name)
{
    auto it = _allContexts.find(name);
    if (_allContexts.end() != it)
    {
        return it->second;
    }

    Ptr<InputContext> context = New<InputContext>();
    context->_name = name;
    _allContexts[name] = context;
    return context;
}

Ptr<class InputAction> InputSystem::FindOrAddInputAction(const std::string& name)
{
    auto it = _allActions.find(name);
    if (_allActions.end() != it)
    {
        return it->second;
    }

    Ptr<InputAction> action = New<InputAction>();
    action->_name = name;
    _allActions[name] = action;
    return action;
}

bool InputSystem::IsActiveContext(const std::string& name)
{
    return _activeContexts.contains(name);
}

uint8 InputSystem::ConvertKey(uint8 key)
{
    return _input->ConvertKey(key);
}

void InputSystem::Destroy()
{
    _activeContexts.clear();
    for (auto& it : _allContexts)
    {
        DESTROY(it.second);
    }
    for (auto& it : _allActions)
    {
        DESTROY(it.second);
    }
    _allContexts.clear();
    _allActions.clear();
}

void InputSystem::RegisterInputComponent(int32 actorID, Ptr<class InputComponent> inputComp)
{
    std::pair<int32, int32> key(actorID, inputComp->GetComponentID());
    _inputComponents[key] = inputComp;
}

void InputSystem::RemoveInputComponent(int32 actorID, int32 componentID)
{
    std::pair<int32, int32> key(actorID, componentID);
    _inputComponents.erase(key);
}

void InputSystem::RefreshInputComponent(const std::string& contextName, const FInputMapping& mapping)
{
    for (auto it : _inputComponents)
    {
        Ptr<InputComponent> comp = Lock<InputComponent>(it.second);
        if (nullptr == comp)
        {
            continue;
        }
        comp->RefreshInputMapping(contextName, mapping);
    }
}

const FVector2D& InputSystem::GetMouseWorldPos() const
{
    return _input->GetMouseWorldPos();
}

const FVector2D& InputSystem::GetMousePos() const
{
    return _input->GetMousePos();
}

const FVector2D& InputSystem::GetMouseMove() const
{
    return _input->GetMouseMove();
}

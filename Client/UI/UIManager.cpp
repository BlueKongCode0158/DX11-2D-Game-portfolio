#include "pch.h"
#include "Widget.h"
#include "UIManager.h"
#include "../Render/RenderManager.h"
#include "../Render/BlendState.h"
#include "../Render/DepthStencilState.h"
#include "../Input/InputSystem.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::MouseHovered(const FVector2D& pos)
{
	Ptr<Widget> currentHit = nullptr;

	for (auto it = _rootWidgets.rbegin(); it != _rootWidgets.rend(); it++)
	{
		if (auto hit = it->second->MouseHit(pos))
		{
			currentHit = hit;
			break;
		}
	}
	
	Ptr<Widget> prevHorved = Lock<Widget>(_hoveredWidget);
	if (currentHit != prevHorved)
	{
		if (prevHorved)
		{
			prevHorved->OnHovered();
		}
	}

}

void UIManager::MousePressed(const FVector2D& pos)
{
}

void UIManager::MouseReleased(const FVector2D& pos)
{
}

bool UIManager::Init(Ptr<class Level> level)
{
	_level = level;
	return true;
}

void UIManager::Tick(float deltaTime)
{
	for (auto it : _removeWidgets)
	{
		auto foundIt = _rootWidgets.find(it);
		if (_rootWidgets.end() == foundIt)
		{
			continue;
		}

		DESTROY(foundIt->second);
		_rootWidgets.erase(it);
	}
	_removeWidgets.clear();

	for (auto& it : _rootWidgets)
	{
		it.second->Tick(deltaTime);
	}
}

void UIManager::Render()
{
	RenderManager::Instance().GetAlphaBlend(eBlendMode::DEFAULT)->SetState();
	RenderManager::Instance().GetDepthStencilState()->SetState();
	for (auto& it : _rootWidgets)
	{
		it.second->Render();
	}
	RenderManager::Instance().GetAlphaBlend(eBlendMode::DEFAULT)->ResetState();
	RenderManager::Instance().GetDepthStencilState()->ResetState();
}

void UIManager::MouseEvent()
{
	FVector2D mousePos = InputSystem::Instance().GetMousePos();
	MouseHovered(mousePos);

}

void UIManager::AddRemove(int32 remove)
{
	_removeWidgets.push_back(remove);
}

void UIManager::AddToViewport(Ptr<class Widget> widget)
{
	_rootWidgets[widget->GetOwnerID()] = widget;
}

void UIManager::Destroy()
{
	for (auto& it : _rootWidgets)
	{
		DESTROY(it.second);
	}
	_rootWidgets.clear();
	_removeWidgets.clear();
}

Ptr<class Level> UIManager::GetLevel() const
{
	return Lock<Level>(_level);
}

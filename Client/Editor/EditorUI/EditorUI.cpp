#include "pch.h"
#include "EditorUI.h"

EditorUI::EditorUI()
{
}

EditorUI::~EditorUI()
{
}

bool EditorUI::Init(const std::string& name)
{
	_name = name;
	SetActive(true);
	SetEnable(true);
	return true;
}

void EditorUI::Tick(float deltaTime)
{
}

void EditorUI::Render(float deltaTime)
{
}


void EditorUI::Destroy()
{
}

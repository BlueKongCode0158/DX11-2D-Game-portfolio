#include "pch.h"
#include "LoadUI.h"

#include "../EditEngine.h"

#include "../../Core/DirectoryManager.h"
#include "../../Core/GameEngine.h"

#include "../../World/Level.h"
#include "../../World/World.h"

LoadUI::LoadUI()
{
}

LoadUI::~LoadUI()
{
}

bool LoadUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(false);

	return true;
}

void LoadUI::Render(float deltaTimee)
{
	ImGui::Begin("Load");

	ImGui::InputText("NewName", _fileName, 64);
	if (ImGui::Button("LOAD_BUTTON"))
	{
		Load(_fileName);
		SetEnable(false);
		memset(_fileName, 0, 64);
	}
	ImGui::End();
}

void LoadUI::Destroy()
{
}

void LoadUI::Load(const std::string& fileName)
{
	auto cachePath = DirectoryManager::Instance().GetCachePath("Resources\\Level");
	if (!cachePath.has_value() || fileName.empty())
	{
		EditEngine::Instance().ShowError("InValid FileName");
		return;
	}

	std::filesystem::path fullPath;
	if (!DirectoryManager::Instance().GetFile(cachePath.value(), fileName, OUT fullPath))
	{
		EditEngine::Instance().ShowError("InValid FileName");
		return;
	}

	std::ifstream loadFile(fullPath, std::ios::binary);
	if (loadFile.fail())
	{
		EditEngine::Instance().ShowError("File Open Erroe");
		return;
	}

	Ptr<Level> level = GET_WORLD->GetCurLevel();
	if (!level)
	{
		EditEngine::Instance().ShowError("InValid Level");
		return;
	}

	level->Load(loadFile);
	loadFile.close();
}

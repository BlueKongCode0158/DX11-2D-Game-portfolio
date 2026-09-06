#include "pch.h"
#include "SaveUI.h"

#include "../EditEngine.h"

#include "../../Core/DirectoryManager.h"
#include "../../Core/GameEngine.h"

#include "../../World/World.h"
#include "../../World/Level.h"

SaveUI::SaveUI()
{
}

SaveUI::~SaveUI()
{
}

bool SaveUI::Init(const std::string& name)
{
    EditorUI::Init(name);
    SetEnable(false);

    return true;
}

void SaveUI::Render(float deltaTime)
{
    EditorUI::Render(deltaTime);

    ImGui::Begin("SAVE");

    ImGui::InputText("NewName", _fileName, FILE_NAME_STR);

    if (ImGui::Button("SAVE_BUTTON"))
    {
        Save(_fileName);
        SetEnable(false);
        memset(_fileName, 0, FILE_NAME_STR);
    }

    ImGui::End();
}

void SaveUI::Destroy()
{
    EditorUI::Destroy();
}

void SaveUI::Save(const std::string& fileName)
{
    auto cachePath = DirectoryManager::Instance().GetCachePath("Resources\\Level");
    if (cachePath.has_value() || fileName.empty())
    {
        EditEngine::Instance().ShowError("InVaild FileName");
        return;
    }

    std::filesystem::path fullPath;
    if (DirectoryManager::Instance().GetFile(cachePath.value(), fileName, OUT fullPath))
    {
        EditEngine::Instance().ShowError("Duplicate FileName");
        return;
    }

    std::ofstream saveFile(fullPath, std::ios::binary);
    if (saveFile.fail())
    {
        EditEngine::Instance().ShowError("File Open Error");
        return;
    }

    Ptr<Level> level = GET_WORLD->GetCurLevel();
    if (!level)
    {
        EditEngine::Instance().ShowError("InValid Level");
        return;
    }

    level->Save(saveFile);
    saveFile.close();
}

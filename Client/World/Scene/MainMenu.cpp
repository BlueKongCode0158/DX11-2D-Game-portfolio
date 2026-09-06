#include "pch.h"
#include "MainMenu.h"

MainMenuScene::MainMenuScene()
{
}

MainMenuScene::~MainMenuScene()
{
}

bool MainMenuScene::Init(const std::string& name)
{
    Level::Init(name);

    return true;
}

void MainMenuScene::Destroy()
{
    Level::Destroy();
}

#pragma once
#include "../Level.h"

class MainMenuScene : public Level
{
public:
	MainMenuScene();
	virtual ~MainMenuScene();
	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) = delete;
	MainMenuScene& operator = (const MainMenuScene& other) = delete;
	MainMenuScene& operator = (MainMenuScene&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
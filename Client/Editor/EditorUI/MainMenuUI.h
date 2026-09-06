#pragma once
#include "EditorUI.h"

class MainMenuUI : public EditorUI
{
public:
	MainMenuUI();
	virtual ~MainMenuUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
private:
	void File();
	void View();
	void Actor();
	void Level();
	void UI();
	void Collision();
};
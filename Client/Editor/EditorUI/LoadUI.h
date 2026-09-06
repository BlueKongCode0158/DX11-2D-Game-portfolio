#pragma once
#include "EditorUI.h"

class LoadUI : public EditorUI
{
public:
	LoadUI();
	virtual ~LoadUI();
private:
	char _fileName[64] = {};
public:
	virtual bool Init(const std::string& name);
	virtual void Render(float deltaTimee);
	virtual void Destroy() override;
private:
	void Load(const std::string& fileName);
};
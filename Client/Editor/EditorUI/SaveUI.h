#pragma once
#include "EditorUI.h"

#define FILE_NAME_STR 32

class SaveUI : public EditorUI
{
public:
	SaveUI();
	virtual ~SaveUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
private:
	void Save(const std::string& fileName);
private:
	char _fileName[FILE_NAME_STR] = {};
};
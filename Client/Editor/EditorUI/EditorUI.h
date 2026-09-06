#pragma once
#include "../../Object/Object.h"

/*
imgui 는 ui를 만들 때 하나하나 begin 과 end 사이에 ui를 만들어줘야한다.
근데 코드 본문에서 그렇게 만들다보면 쓸모없이 줄이 길어지기때문에
EditorUI를 통해 매핑을 해주는 것.
*/
class EditorUI : public Object
{
public:
	EditorUI();
	virtual ~EditorUI();
public:
	virtual bool Init(const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void Destroy() override;
protected:
	std::string _name;
};
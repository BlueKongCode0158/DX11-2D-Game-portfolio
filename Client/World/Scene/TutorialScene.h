#pragma once
#include "../Level.h"

class TutorialScene : public Level
{
public:
	TutorialScene();
	virtual ~TutorialScene();
	TutorialScene(const TutorialScene& other) = delete;
	TutorialScene(TutorialScene&& other) = delete;
	TutorialScene& operator = (const TutorialScene& other) = delete;
	TutorialScene& operator = (TutorialScene&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
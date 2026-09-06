#pragma once
#include "../Level.h"

class TownScene : public Level
{
public:
	TownScene();
	virtual ~TownScene();
	TownScene(const TownScene& other) = delete;
	TownScene(TownScene&& other) = delete;
	TownScene& operator = (const TownScene& other) = delete;
	TownScene& operator = (TownScene&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
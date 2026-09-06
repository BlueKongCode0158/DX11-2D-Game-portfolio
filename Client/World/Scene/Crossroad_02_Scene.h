#pragma once
#include "../Level.h"

class Crossroad_02 : public Level
{
public:
	Crossroad_02();
	virtual ~Crossroad_02();
	Crossroad_02(const Crossroad_02& other) = delete;
	Crossroad_02(Crossroad_02&& other) = delete;
	Crossroad_02& operator = (const Crossroad_02& other) = delete;
	Crossroad_02& operator = (Crossroad_02&& other) = delete;
public:
	virtual bool Init(const std::string& path) override;
public:
	virtual void Destroy() override;
};
#pragma once
#include "../Level.h"

class Crossroad_01 : public Level
{
public:
	Crossroad_01();
	virtual ~Crossroad_01();
	Crossroad_01(const Crossroad_01& other) = delete;
	Crossroad_01(Crossroad_01&& other) = delete;
	Crossroad_01& operator = (const Crossroad_01& other) = delete;
	Crossroad_01& operator = (Crossroad_01&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
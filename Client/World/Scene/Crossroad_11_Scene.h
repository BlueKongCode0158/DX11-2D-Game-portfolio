#pragma once
#include "../Level.h"

class Crossroad_11 : public Level
{
public:
	Crossroad_11();
	virtual ~Crossroad_11();
	Crossroad_11(const Crossroad_11& other) = delete;
	Crossroad_11(Crossroad_11&& other) = delete;
	Crossroad_11& operator = (const Crossroad_11& other) = delete;
	Crossroad_11& operator = (Crossroad_11&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
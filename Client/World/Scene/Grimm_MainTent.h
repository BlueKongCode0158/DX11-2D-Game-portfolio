#pragma once
#include "../Level.h"

class Grimm_MainTent : public Level
{
public:
	Grimm_MainTent();
	virtual ~Grimm_MainTent();
public:
	virtual bool Init(const std::string& path) override;
	virtual void Destroy() override;
};
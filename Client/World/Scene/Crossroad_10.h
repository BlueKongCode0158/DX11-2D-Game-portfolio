#pragma once
#include "../Level.h"

class Crossrad_10_Scene : public Level
{
public:
	Crossrad_10_Scene() = default;
	virtual ~Crossrad_10_Scene() = default;
	Crossrad_10_Scene(const Crossrad_10_Scene& other) = delete;
	Crossrad_10_Scene(Crossrad_10_Scene&& other) = delete;
	Crossrad_10_Scene& operator = (const Crossrad_10_Scene& other) = delete;
	Crossrad_10_Scene& operator = (Crossrad_10_Scene&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
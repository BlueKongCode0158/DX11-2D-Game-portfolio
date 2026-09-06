#pragma once
#include "../Level.h"

class FinalBoss_Street : public Level
{
public:
	FinalBoss_Street();
	virtual ~FinalBoss_Street();
	FinalBoss_Street(const FinalBoss_Street& other) = delete;
	FinalBoss_Street(FinalBoss_Street&& other) = delete;
	FinalBoss_Street& operator = (const FinalBoss_Street& other) = delete;
	FinalBoss_Street& operator = (FinalBoss_Street&& other) = delete;
public:
	virtual bool Init(const std::string& name) override;
public:
	virtual void Destroy() override;
};
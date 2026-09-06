#pragma once
#include "CBuffer.h"

class ProgressBarCBuffer : public CBuffer
{
public:
	ProgressBarCBuffer();
	virtual ~ProgressBarCBuffer();
private:
	FProgressBarData _data;
public:
	virtual void Update() override;
public:
	void SetPercent(float percent)
	{
		_data._percent = percent;
	}

	void SetDir(int32 dir)
	{
		_data._barDir = dir;
	}
};
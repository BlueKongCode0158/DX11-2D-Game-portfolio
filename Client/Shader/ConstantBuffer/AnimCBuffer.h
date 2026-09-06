#pragma once
#include "CBuffer.h"
#include "../CBufferData.h"

class AnimCBuffer : public CBuffer
{
public:
	AnimCBuffer();
	virtual ~AnimCBuffer();
private:
	FAnimationCBufferData _data;
public:
	virtual void Update() override;
	void SetUV(float ltx, float lty, float rbx, float rby);
	void SetOffset(float x, float y);
	void SetAnimRotated(bool rotate);
	void SetAnimFlip(bool flip);
};
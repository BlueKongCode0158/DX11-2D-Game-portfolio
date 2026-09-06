#pragma once
#include "CBuffer.h"
#include "../CBufferData.h"

class SpriteCBuffer : public CBuffer
{
public:
	SpriteCBuffer();
	virtual ~SpriteCBuffer();
private:
	FSpriteCBufferData _data;
public:
	virtual void Update() override;
	virtual void Destroy() override;
public:
	void SetTint(float r, float g, float b, float a);
	void SetParallaxOffset(float x, float y);
	void SetParallaxScale(float scale);
	void SetParallaxPivot(float x, float y);
	void SetTint(const FVector4D& color);
};
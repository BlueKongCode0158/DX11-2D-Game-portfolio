#pragma once
#include "CBuffer.h"
#include "../CBufferData.h"

class MaterialCBuffer : public CBuffer
{
public:
	MaterialCBuffer();
	virtual ~MaterialCBuffer();
private:
	FMaterialCBufferData _data;
public:
	virtual void Update() override;
	virtual void Destroy() override;
public:
	void SetBaseColor(float r, float g, float b, float a);
	void SetBaseColor(const FVector4D& color);
	void SetOpacity(int32 op);
	void SetTextureWidth(int32 width);
	void SetTextureHeight(int32 height);
};
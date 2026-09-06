#pragma once
#include "Shader/ConstantBuffer/CBuffer.h"
#include "Shader/CBufferData.h"

class TransformCBuffer : public CBuffer
{
public:
	TransformCBuffer();
	virtual ~TransformCBuffer();
public:
	virtual void Update();
	void SetWorldMatrix(const FMatrix& mat);
	void SetViewMatrix(const FMatrix& mat);
	void SetProjMatrix(const FMatrix& mat);
private:
	FTransformCBufferData _data;
};


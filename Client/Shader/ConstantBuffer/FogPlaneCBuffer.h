#pragma once
#include "Shader/ConstantBuffer/CBuffer.h"
#include "Shader/CBufferData.h"

class FogPlaneCBuffer : public CBuffer
{
public:
	FogPlaneCBuffer();
	virtual ~FogPlaneCBuffer();
public:
	virtual void Update();
public:
	void SetScreeSize(float w, float h)
	{
		_data._screemSize._x = w;
		_data._screemSize._y = h;
	}
	void SetTime(float t)
	{
		_data._Time = t;
	}
	void SetDistorition(float dist)
	{
		_data._Dist = dist;
	}
	void SetTint(float r, float g, float b, float w)
	{
		_data._tint._x = r;
		_data._tint._y = g;
		_data._tint._z = b;
		_data._tint._w = w;
	}
	void SetTint(FVector4D tint)
	{
		SetTint(tint._x, tint._y, tint._z, tint._w);
	}
private:
	FFogPlaneData _data;
};


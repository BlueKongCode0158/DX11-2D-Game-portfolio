#pragma once
#include "CBuffer.h"
#include "Shader/CBufferData.h"

class ColorCBuffer : public CBuffer
{
public:
	ColorCBuffer() = default;
	virtual ~ColorCBuffer() = default;
public:
	virtual void Update();
public:
	void SetColor(float r, float g, float b, float a)
	{
		_data._color._x = r;
		_data._color._y = g;
		_data._color._z = b;
		_data._color._w = a;
	}
private:
	FColorCBufferData _data;
};


#include "pch.h"
#include "MaterialCBuffer.h"

MaterialCBuffer::MaterialCBuffer()
{
}

MaterialCBuffer::~MaterialCBuffer()
{
}

void MaterialCBuffer::Update()
{
	SetData(&_data);
}

void MaterialCBuffer::Destroy()
{
}

void MaterialCBuffer::SetBaseColor(float r, float g, float b, float a)
{
	_data._baseColor._x = r;
	_data._baseColor._y = g;
	_data._baseColor._z = b;
	_data._baseColor._w = a;
}

void MaterialCBuffer::SetBaseColor(const FVector4D& color)
{
	SetBaseColor(color._x, color._y, color._z, color._w);
}

void MaterialCBuffer::SetOpacity(int32 op)
{
	_data._opacity = op;
}

void MaterialCBuffer::SetTextureWidth(int32 width)
{
	_data._textureWidth = width;
}

void MaterialCBuffer::SetTextureHeight(int32 height)
{
	_data._textureHeight = height;
}
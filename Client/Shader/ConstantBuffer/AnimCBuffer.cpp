#include "pch.h"
#include "AnimCBuffer.h"

AnimCBuffer::AnimCBuffer()
{
}

AnimCBuffer::~AnimCBuffer()
{
}

void AnimCBuffer::Update()
{
	SetData(&_data);
}

void AnimCBuffer::SetUV(float ltx, float lty, float rbx, float rby)
{
	_data._LTUV._x = ltx;
	_data._LTUV._y = lty;
	_data._RBUV._x = rbx;
	_data._RBUV._y = rby;
}

void AnimCBuffer::SetOffset(float x, float y)
{
	_data._offset._x = x;
	_data._offset._y = y;
}

void AnimCBuffer::SetAnimRotated(bool rotate)
{
	_data._animRotated90 = rotate;
}

void AnimCBuffer::SetAnimFlip(bool flip)
{
	_data._animFlip = flip;
}

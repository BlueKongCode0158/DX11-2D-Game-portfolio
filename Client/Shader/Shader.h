#pragma once
#include "Object.h"

class Shader : public Object
{
public:
	Shader() = default;
	virtual ~Shader() = default;
public:
	virtual bool Init() = 0;
	virtual void SetShader() = 0;
};


#pragma once
#include "GraphicShader.h"

class UIShader : public GraphicShader
{
public:
	UIShader();
	virtual ~UIShader();
public:
	virtual bool Init() override;
};
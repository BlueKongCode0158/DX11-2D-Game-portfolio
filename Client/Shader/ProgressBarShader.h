#pragma once
#include "GraphicShader.h"

class ProgressBarShader : public GraphicShader
{
public:
	ProgressBarShader();
	virtual ~ProgressBarShader();
public:
	virtual bool Init() override;
};
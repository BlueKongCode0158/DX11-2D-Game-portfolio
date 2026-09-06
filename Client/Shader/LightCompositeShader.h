#pragma once
#include "GraphicShader.h"

class LightCompositeShader : public GraphicShader
{
public:
	LightCompositeShader();
	virtual ~LightCompositeShader();
public:
	virtual bool Init() override;
};
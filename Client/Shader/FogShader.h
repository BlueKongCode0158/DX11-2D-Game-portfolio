#pragma once
#include "GraphicShader.h"

class FogShader : public GraphicShader
{
public:
	FogShader();
	virtual ~FogShader();
public:
	virtual bool Init() override;
	virtual void Destroy() override;
};
#pragma once
#include "GraphicShader.h"

class FogPlaneShader : public GraphicShader
{
public:
	FogPlaneShader();
	virtual ~FogPlaneShader();
public:
	virtual bool Init() override;
	virtual void Destroy() override;
};
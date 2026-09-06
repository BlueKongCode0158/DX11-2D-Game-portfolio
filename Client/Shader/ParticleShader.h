#pragma once
#include "GraphicShader.h"

class ParticleShader : public GraphicShader
{
public:
	ParticleShader();
	virtual ~ParticleShader();
public:
	virtual bool Init() override;
	virtual void Destroy() override;
};
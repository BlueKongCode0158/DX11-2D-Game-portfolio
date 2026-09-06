#pragma once
#include "GraphicShader.h"

class SpriteBatchShader : public GraphicShader
{
public:
	SpriteBatchShader();
	virtual ~SpriteBatchShader();
public:
	virtual bool Init() override;
	virtual void Destroy() override;
};
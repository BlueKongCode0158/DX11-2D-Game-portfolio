#pragma once
#include "GraphicShader.h"

class SpriteShader : public GraphicShader
{
public:
	SpriteShader();
	virtual ~SpriteShader();
public:
	virtual void Destroy() override;
	virtual bool Init() override;
};
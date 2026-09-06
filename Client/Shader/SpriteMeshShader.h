#pragma once
#include "GraphicShader.h"

class SpriteMeshShader : public GraphicShader
{
public:
	SpriteMeshShader();
	virtual ~SpriteMeshShader();
public:
	virtual bool Init() override;
	virtual void Destroy() override;
};
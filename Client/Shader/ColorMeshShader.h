#pragma once
#include "GraphicShader.h"

class ColorMeshShader : public GraphicShader
{
public:
	ColorMeshShader();
	~ColorMeshShader();
public:
	virtual bool Init() override;
};


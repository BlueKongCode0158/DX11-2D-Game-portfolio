#pragma once
#include "Shader/GraphicShader.h"

class FrameMeshShader : public GraphicShader
{
public:
	FrameMeshShader() = default;
	virtual ~FrameMeshShader() = default;
public:
	virtual bool Init() override;
};
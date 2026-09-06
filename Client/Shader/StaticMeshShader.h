#pragma once
#include "GraphicShader.h"

class StaticMeshShader : public GraphicShader
{
public:
	StaticMeshShader();
	virtual ~StaticMeshShader();
public:
	bool Init();
	virtual void Destroy() override;
};
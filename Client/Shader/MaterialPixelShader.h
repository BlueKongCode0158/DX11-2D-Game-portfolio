#pragma once
#include "Shader.h"

class MaterialPixelShader : public Shader
{
public:
	MaterialPixelShader();
	virtual ~MaterialPixelShader();
private:
	ComPtr<ID3D11PixelShader> _ps = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;
public:
	virtual bool Init() override;
	virtual void SetShader() final;
	bool LoadPixelShader(const char* entryName, const char* fileName);
	virtual void Destroy() override;
};
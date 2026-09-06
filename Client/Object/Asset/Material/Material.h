#pragma once
#include "Asset.h"

struct FMaterialTextureInfo
{
	std::string _name = "";
	Weak<class Texture> _texture;
	int32 _registerNum = 0;
	int32 _shaderBufferType;
	int32 _textureIndex;
};

class Material : public Asset
{
	friend class MaterialManager;
public:
	Material();
	virtual ~Material();
protected:
	std::vector<FMaterialTextureInfo> _textures;
	Ptr<class Shader> _pixelShader;
	Ptr<class MaterialCBuffer> _buffer;
	eTextureSampleType _sampleType = eTextureSampleType::TEXTURE_SAMPLE_LINEAR;
	FVector4D _baseColor = FVector4D(0.f, 0.f, 0.f, 1.f);
	float _opacity = 1.f;
public:
	void SetBaseColor(float r, float g, float b, float a);
	void SetBaseColor(const FVector4D& color);
	void SetOpacity(float op);
	void SetSamplerType(eTextureSampleType type);
public:
	void AddTexture(Ptr<class Texture> texture, int32 registerNum, int32 shaderBufferType, int32 textureIndex);
	void AddTexture(const std::string& name, int32 registerNum, int32 shaderBufferType, int32 textureIndex);
	void SetPixelShader(const std::string& name);
public:
	void SetMaterial();
	void ResetMaterial();
public:
	Ptr<Material> Clone();
public:
	virtual void Destroy() override;
};
#include "pch.h"
#include "Material.h"
#include "../AssetManager.h"
#include "../Texture/Texture.h"
#include "../../../Shader/Shader.h"
#include "../../../Shader/ShaderManager.h"
#include "../../../Shader/ConstantBuffer/MaterialCBuffer.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::SetBaseColor(float r, float g, float b, float a)
{
	_baseColor._x = r;
	_baseColor._y = g;
	_baseColor._z = b;
	_baseColor._w = a;
}

void Material::SetBaseColor(const FVector4D& color)
{
	SetBaseColor(color._x, color._y, color._z, color._w);
}

void Material::SetOpacity(float op)
{
	_opacity = op;
}

void Material::SetSamplerType(eTextureSampleType type)
{
	_sampleType = type;
}

void Material::AddTexture(Ptr<class Texture> texture, int32 registerNum, int32 shaderBufferType, int32 textureIndex)
{
	FMaterialTextureInfo info;
	info._texture = texture;
	info._registerNum = registerNum;
	info._shaderBufferType = shaderBufferType;
	info._textureIndex = textureIndex;

	_buffer->SetTextureWidth(texture->GetTexture(textureIndex)->_width);
	_buffer->SetTextureHeight(texture->GetTexture(textureIndex)->_height);

	_textures.emplace_back(info);
}

void Material::AddTexture(const std::string& name, int32 registerNum, int32 shaderBufferType, int32 textureIndex)
{
	Ptr<Texture> texture = TEXTURE_MANAGER->FindTexture(name);
	if (nullptr == texture)
	{
		return;
	}

	FMaterialTextureInfo info;
	info._texture = texture;
	info._registerNum = registerNum;
	info._shaderBufferType = shaderBufferType;
	info._textureIndex = textureIndex;
	info._name = texture->GetName();

	_buffer->SetTextureWidth(texture->GetTexture(textureIndex)->_width);
	_buffer->SetTextureHeight(texture->GetTexture(textureIndex)->_height);

	_textures.emplace_back(info);
}

void Material::SetPixelShader(const std::string& name)
{
	_pixelShader = ShaderManager::Instance().FindShader(name);
}

void Material::SetMaterial()
{
	_buffer->SetBaseColor(_baseColor);
	_buffer->SetOpacity(_opacity);
	_buffer->Update();
	ShaderManager::Instance().SetSample(_sampleType);
	
	_pixelShader->SetShader();
	size_t texCount = _textures.size();
	for (size_t i = 0; i < texCount; i++)
	{
		Ptr<Texture> texture = Lock<Texture>(_textures[i]._texture);
		if (nullptr == texture)
		{
			continue;
		}
		texture->SetShader(_textures[i]._registerNum, _textures[i]._shaderBufferType, _textures[i]._textureIndex);
	}
}

void Material::ResetMaterial()
{
	size_t texCount = _textures.size();
	for (size_t i = 0; i < texCount; i++)
	{
		Ptr<Texture> texture = Lock<Texture>(_textures[i]._texture);
		if (nullptr == texture)
		{
			continue;
		}
		texture->ResetShader(_textures[i]._registerNum, _textures[i]._shaderBufferType);
	}
}

Ptr<Material> Material::Clone()
{
	Ptr<Material> clone = New<Material>();
	clone->_baseColor = _baseColor;
	clone->_opacity = _opacity;
	clone->_sampleType = _sampleType;
	clone->_pixelShader = _pixelShader;
	clone->_buffer = _buffer;
	clone->_textures = _textures;
	return clone;
}

void Material::Destroy()
{
}

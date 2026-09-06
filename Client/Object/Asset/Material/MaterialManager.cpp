#include "pch.h"
#include "MaterialManager.h"
#include "../../../Shader/ConstantBuffer/MaterialCBuffer.h"
#include "../../../Shader/ShaderManager.h"
#include "Material.h"

bool MaterialManager::Init()
{
	if (false == CreateMaterial("DefaultMaterial", "MaterialPixelShader", eTextureSampleType::TEXTURE_SAMPLE_LINEAR))
	{
		return false;
	}

	// 색상 전용 메시(FVertexColor, TEXCOORD 없음)용 머티리얼. 픽셀셰이더로 ColorMeshShader 를
	// 지정하면 SetMaterial 시 ColorMesh VS/PS 가 함께 바인딩되어, 기본 MaterialPixelShader
	// (TEXCOORD 요구)로 PS 가 덮어써져 VS/PS 시그니처가 깨지는 문제(#342)를 막는다.
	if (false == CreateMaterial("ColorMeshMaterial", "ColorMeshShader", eTextureSampleType::TEXTURE_SAMPLE_LINEAR))
	{
		return false;
	}
	return true;
}

bool MaterialManager::CreateMaterial(const std::string& name, const std::string& pixelShader, eTextureSampleType sampleType)
{
	if (FindMaterial(name))
	{
		return false;
	}
	Ptr<Material> mat = New<Material>();
	mat->SetName(name);
	mat->SetSamplerType(sampleType);
	mat->SetPixelShader(pixelShader);

	mat->_buffer = ShaderManager::Instance().FindCBuffer<MaterialCBuffer>("Material");
	_materials[name] = mat;

	return true;
}

Ptr<class Material> MaterialManager::CreateMaterialInstance(const std::string& name)
{
	Ptr<Material> origin = FindMaterial(name);
	if (nullptr == origin)
	{
		return nullptr;
	}

	return origin->Clone();
}

Ptr<class Material> MaterialManager::FindMaterial(const std::string& name)
{
	auto it = _materials.find(name);
	if (_materials.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

void MaterialManager::Destroy()
{
	for (auto& it : _materials)
	{
		DESTROY(it.second);
	}
}

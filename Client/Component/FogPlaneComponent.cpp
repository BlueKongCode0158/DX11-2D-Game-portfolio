#include "pch.h"
#include "FogPlaneComponent.h"

#include "../Shader/FogPlaneShader.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Shader/ConstantBuffer/FogPlaneCBuffer.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"

#include "../Object/Asset/AssetManager.h"
#include "../Shader/ShaderManager.h"

#include "../Core/Device.h"
#include "../World/Level.h"
#include "../Component/CameraComponent.h"

#include "../Object/Asset/Texture/Texture.h"

#include "../Editor/EditEngine.h"

FogPlaneComponent::FogPlaneComponent()
{
	_isRender = true;
}

FogPlaneComponent::~FogPlaneComponent()
{
}

bool FogPlaneComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	//SetRenderLayerName("FogPlane");
	SceneComponent::Init(id, name, owner);

	_mesh = MESH_MANAGER->FindMesh("TexRect");
	_shader = ShaderManager::Instance().FindShader("FogPlaneShader");
	_fogPlaneCBuffer = ShaderManager::Instance().FindCBuffer<FogPlaneCBuffer>("FogPlane");
	_noiseTexture = TEXTURE_MANAGER->FindTexture("Noise");

	_type = eComponentType::FOGPLANE;
	return true;
}

void FogPlaneComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);
	Device::Instance().GrabPass();
	
	_time += deltaTime * 2.f;

	Ptr<Level> level = Lock<Level>(_level);

	if (nullptr == level)
	{
		return;
	}

	_transformCBuffer->SetWorldMatrix(_matrix._world);
	_transformCBuffer->SetViewMatrix(level->GetViewMatrix());
	_transformCBuffer->SetProjMatrix(level->GetProjMatrix());
	_transformCBuffer->Update();

	_fogPlaneCBuffer->SetScreeSize(static_cast<float>(Device::Instance().GetRS()._width), static_cast<float>(Device::Instance().GetRS()._height));
	_fogPlaneCBuffer->SetTime(_time);
	_fogPlaneCBuffer->SetTint(_tint);
	_fogPlaneCBuffer->SetDistorition(_dist);
	
	_fogPlaneCBuffer->Update();

	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
	_shader->SetShader();

	ComPtr<ID3D11ShaderResourceView> grabSRV = Device::Instance().GetGrabSRV();
	ID3D11ShaderResourceView* srv = Device::Instance().GetGrabSRV().Get();

	Device::Instance().GetContext()->PSSetShaderResources(0, 1, &srv);

	if (_noiseTexture)
		_noiseTexture->SetShader(1, SHADER_TYPE::PIXEL, 0);

	_mesh->Render();
	
	ID3D11ShaderResourceView* nullSRV = nullptr;
	Device::Instance().GetContext()->PSSetShaderResources(0, 1, &nullSRV);
	Device::Instance().GetContext()->PSSetShaderResources(1, 1, &nullSRV);
}

void FogPlaneComponent::DrawInspector()
{
	SceneComponent::DrawInspector();

	ImGui::SeparatorText("FogPlne");

	ImGui::SeparatorText("Tint");
	if (ImGui::ColorEdit4("Tint", &_tint._x))
	{
		SetTint(_tint);
	}

	if (ImGui::DragFloat("Distortion", &_dist, 0.02f, 0.1f, 50.f))
	{
		_fogPlaneCBuffer->SetDistorition(_dist);
	}
}

void FogPlaneComponent::Destroy()
{
	SceneComponent::Destroy();

}

bool FogPlaneComponent::Save(std::ofstream& file)
{
	return true;
}

bool FogPlaneComponent::Load(std::ifstream& file)
{
	return true;
}

void FogPlaneComponent::SetTint(FVector4D tint)
{
	SetTint(tint._x, tint._y, tint._z, tint._w);
}

void FogPlaneComponent::SetTint(float r, float g, float b, float w)
{
	_tint._x = r;
	_tint._y = g;
	_tint._z = b;
	_tint._w = w;
}

void FogPlaneComponent::SetOp(float w)
{
	_tint._w = w;
}

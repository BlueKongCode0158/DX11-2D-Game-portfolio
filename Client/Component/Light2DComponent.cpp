#include "pch.h"
#include "Light2DComponent.h"
#include "../Object/Asset/AssetManager.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Object/Asset/Texture/Texture.h"
#include "../Render/RenderManager.h"

Light2DComponent::Light2DComponent()
{
	_isRender = true;
	_renderLayerName = "BackgroundLight";
}

Light2DComponent::~Light2DComponent()
{
}

bool Light2DComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	//if (GetWorldPosition()._z >= -PLAYER_DEADZONE && GetWorldPosition()._z <= PLAYER_DEADZONE) 
	//{
	//	SetRenderLayerName("Default");
	//}
	//else if (GetWorldPosition()._z >= PLAYER_DEADZONE) 
	//{
	//	SetRenderLayerName("BackgroundLight");
	//}
	//else
	//{
	//	SetRenderLayerName("ForegroundLight");
	//}

	return true;
}

void Light2DComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
}

void Light2DComponent::Destroy()
{
	SceneComponent::Destroy();
}

void Light2DComponent::SetMesh(const std::string& meshName, int32 pathID)
{
	_mesh = MESH_MANAGER->FindMesh(meshName, pathID);
	MarkRenderBoundDirty();
}

void Light2DComponent::SetTexture(const std::string& textureName, int32 textureID)
{
	_texture = TEXTURE_MANAGER->FindTexture(textureName);
	_textureIndex = textureID;
}

void Light2DComponent::SetTint(const FVector4D& tint)
{
	_tint = tint;
}

void Light2DComponent::SetIntensity(float intensity)
{
	_intensity = intensity;
}

bool Light2DComponent::BuildLightRenderProxy(OUT FLightRenderProxy& proxy) const
{
	if (!_mesh || !_texture)
	{
		return false;
	}

	proxy._world = GetWorldMatrix();
	proxy._mesh = _mesh;
	proxy._texture = _texture;
	proxy._textureIndex = _textureIndex;
	proxy._tint = _tint;
	proxy._intensity = _intensity;
	return true;
}

bool Light2DComponent::BuildRenderBound2D(OUT FLocalBound2D& out) const
{
	if (!_mesh)
	{
		return false;
	}

	const FLocalBound2D& local = _mesh->GetLocalBound();
	return BuildBoundFromLocal(local, out);
}

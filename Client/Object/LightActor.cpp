#include "pch.h"
#include "LightActor.h"
#include "../Component/Light2DComponent.h"

LightActor::LightActor()
{
}

LightActor::~LightActor()
{
}

bool LightActor::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);
	_type = eActorType::Light;

	_meshComp = CreateSceneComponent<Light2DComponent>("LightMesh");
	_meshComp->SetWorldScale(64.f, 64.f, 1.f);
	_meshComp->AttachToComponent(_root);
	_meshComp->SetRenderLayerName("BackgroundLight");

	return true;
}

void LightActor::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void LightActor::Render(float deltaTime)
{
	Actor::Render(deltaTime);
}

void LightActor::Destroy()
{
	Actor::Destroy();
}

void LightActor::SetMesh(const std::string& meshName, int32 pathID)
{
	if (!_meshComp)
	{
		return;
	}
	_meshComp->SetMesh(meshName, pathID);
}

void LightActor::SetTexture(const std::string& textureName, int32 textureID)
{
	if (!_meshComp)
	{
		return;
	}
	_meshComp->SetTexture(textureName, textureID);
}

void LightActor::SetTint(const FVector4D& tint)
{
	if (!_meshComp)
	{
		return;
	}
	_meshComp->SetTint(tint);
}

void LightActor::SetIntensity(float intensity)
{
	if (!_meshComp)
	{
		return;
	}
	_meshComp->SetIntensity(intensity);
}

#include "pch.h"
#include "FogPlane.h"

#include "../Component/FogPlaneComponent.h"

bool FogPlane::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);
	_type = eActorType::Fog;

	auto fog = CreateSceneComponent<FogPlaneComponent>("FogPlane");
	fog->SetRelativePosition({ 0.f, 0.f, 0.f });    // 화면 중앙
	fog->SetRelativeScale({ 1280.f, 720.f, 1.f });   // 200x200 크기 (값은 단위계에 맞춰서)
	fog->AttachToComponent(_root);

	return true;
}

void FogPlane::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void FogPlane::Render(float deltaTime)
{
	Actor::Render(deltaTime);
}

void FogPlane::Destroy()
{
	Actor::Destroy();
}

bool FogPlane::Save(std::ofstream& file)
{
	Actor::Save(file);
	return true;
}

bool FogPlane::Load(std::ifstream& file)
{
	Actor::Load(file);
	return true;
}

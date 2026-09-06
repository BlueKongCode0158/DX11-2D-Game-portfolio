#include "pch.h"
#include "Crossroad_11_Scene.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"

#include "../../Component/CameraComponent.h"
Crossroad_11::Crossroad_11()
{
}

Crossroad_11::~Crossroad_11()
{
}

bool Crossroad_11::Init(const std::string& name)
{
	Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(3300.f, 920.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(1.f, 1.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 4.6f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);

	LoadProbInstance(L"Crossroads_11_alt_StaticProbInstances.json");
	LoadLineColliders(L"Crossroads_11_alt_SceneChunkMap.json");
	LoadLightObject(L"Crossroads_11_alt_LightMaskObjects.json");
	LoadColliderObject(L"Crossroads_11_alt_SceneColliderObjects.json");
	LoadCameraLock(L"Crossroads_11_alt_SceneCameraLockZones.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);
	return true;
}

void Crossroad_11::Destroy()
{
	Level::Destroy();

}

#include "pch.h"
#include "Crossroad_10.h"

#include "../../Component/CameraComponent.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"

bool Crossrad_10_Scene::Init(const std::string& name)
{
    Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(1524.f, 300.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(1.f, 1.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 4.6f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);

	LoadProbInstance(L"Crossroads_10_boss_defeated_StaticProbInstances.json");
	LoadProbInstance(L"Crossroads_10_boss_StaticProbInstances.json");
	LoadProbInstance(L"Crossroads_10_StaticProbInstances.json");

	LoadLineColliders(L"Crossroads_10_SceneChunkMap.json");
	LoadLightObject(L"Crossroads_10_LightMaskObjects.json");

	LoadColliderObject(L"Crossroads_10_SceneColliderObjects.json");

	LoadCameraLock(L"Crossroads_10_boss_SceneCameraLockZones.json");
	LoadCameraLock(L"Crossroads_10_SceneCameraLockZones.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);

    return true;
}

void Crossrad_10_Scene::Destroy()
{
    Level::Destroy();
}

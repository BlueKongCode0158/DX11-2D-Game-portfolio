#include "pch.h"
#include "Crossroad_01_Scene.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"
#include "../../Component/CameraComponent.h"

Crossroad_01::Crossroad_01()
{
}

Crossroad_01::~Crossroad_01()
{
}

bool Crossroad_01::Init(const std::string& name)
{
	Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(3300.f, 920.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(100.f, 100.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 6.62f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);
	LoadProbInstance(L"Crossroads_01_StaticProbInstances.json");
	LoadLineColliders(L"Crossroads_01_SceneChunkMap.json");
	LoadLightObject(L"Crossroads_01_LightMaskObjects.json");
	//LoadAnimationActor(L"Crossroads_01_AnimationObjects.json");
	LoadColliderObject(L"Crossroads_01_SceneColliderObjects.json");
	LoadCameraLock(L"Crossroads_01_SceneCameraLockZones.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);

	return true;
}

void Crossroad_01::Destroy()
{
	Level::Destroy();

}

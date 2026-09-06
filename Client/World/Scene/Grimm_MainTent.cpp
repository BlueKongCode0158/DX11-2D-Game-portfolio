#include "pch.h"
#include "Grimm_MainTent.h"

#include "../../Object/Grimm.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"

#include "../../Component/CameraComponent.h"

Grimm_MainTent::Grimm_MainTent()
{
}

Grimm_MainTent::~Grimm_MainTent()
{
}

bool Grimm_MainTent::Init(const std::string& path)
{
    Level::Init(path);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(930.f, 420.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(100.f, 100.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 300.f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);

	FVector3D grimmScale = FVector3D(1.f, 1.f, 1.f);
	FVector3D grimmPos = FVector3D(5691.f, 450.f, 0.f);
	Ptr<Grimm> t3 = SpawnActor<Grimm>("Grimm", grimmPos, grimmScale, rot);

	LoadProbInstance(L"Grimm_Main_Tent_StaticProbInstances.json");
	LoadLightObject(L"Grimm_Main_Tent_LightMaskObjects.json");
	//LoadAnimationActor(L"Grimm_Main_Tent_AnimationObjects.json");
	LoadLineColliders(L"Grimm_Main_Tent_SceneChunkMap.json");
	LoadColliderObject(L"Grimm_Main_Tent_SceneColliderObjects.json");
	LoadCameraLock(L"Grimm_Main_Tent_SceneCameraLockZones.json");
	
	//LoadProbInstance(L"Grimm_Nightmare_StaticProbInstances.json");
	//LoadLightObject(L"Grimm_Nightmare_LightMaskObjects.json");
	//LoadAnimationActor(L"Grimm_Nightmare_AnimationObjects.json");
	//LoadLineColliders(L"Grimm_Nightmare_SceneChunkMap.json");
	//LoadColliderObject(L"Grimm_Nightmare_SceneColliderObjects.json");
	//LoadCameraLock(L"Grimm_Nightmare_SceneCameraLockZones.json");

	//LoadAnimationActor(L"Grimm_Nightmare_AnimationObjects.json");
	//LoadLightObject(L"Grimm_Main_Tent_boss_LightMaskObjects.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);
    return true;
}

void Grimm_MainTent::Destroy()
{
    Level::Destroy();
}

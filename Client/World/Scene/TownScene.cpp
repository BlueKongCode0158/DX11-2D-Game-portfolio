#include "pch.h"
#include "TownScene.h"

#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"
#include "../../Object/Gate.h"
#include "../../Component/CameraComponent.h"

TownScene::TownScene()
{
}

TownScene::~TownScene()
{
}

bool TownScene::Init(const std::string& name)
{
	Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(1900.f, 2000.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(100.f, 100.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 300.f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);

	Ptr<Gate> gateActor = SpawnActor<Gate>(
		"GATE", { 11840.f, -96.f, -2.f }, { 200.f, 100.f, 1.f },
		{ 0.f, 0.f, 0.f });

	gateActor->SetLevelType(eLevelType::CROSSROAD_01);
	gateActor->SetGateType(GATETYPE::IMMEDIATE);



	LoadProbInstance(L"Town_StaticProbInstances.json");
	LoadLightObject(L"Town_LightMaskObjects.json");
	LoadAnimationActor(L"Town_AnimationObjects.json");
	LoadLineColliders(L"Town_SceneChunkMap.json");
	LoadCameraLock(L"Town_SceneCameraLockZones.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);

	return true;
}

void TownScene::Destroy()
{
	Level::Destroy();
}
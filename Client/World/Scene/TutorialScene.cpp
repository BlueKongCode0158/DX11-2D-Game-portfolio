#include "pch.h"
#include "TutorialScene.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"
#include "../../Object/Prob.h"
#include "../../Component/ParticleComponent.h"
#include "../../Component/CameraComponent.h"
#include "../../Object/Actor.h"
#include "../../Object/Gate.h"
#include "../../Object/Buzzer.h"
#include "../../Object/Crawler.h"

// 파티클 프리셋(MakeSmokePreset / MakeDustPreset / MakeFirePreset)은
// ParticleComponent.h 에 선언, ParticleComponent.cpp 에 정의되어 있다.

TutorialScene::TutorialScene()
{
}

TutorialScene::~TutorialScene()
{
}

bool TutorialScene::Init(const std::string& name)
{
	Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(2200.f, 4500.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(100.f, 100.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f,1000.f, 13.7f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);


	Ptr<Actor> smokeActor = SpawnActor<Actor>(
		"BG_Smoke", { 2000.f, 800.f, -2.f }, { 100.f, 100.f, 1.f },
		{ 0.f, 0.f, 0.f });

	Ptr<Gate> gateActor = SpawnActor<Gate>(
		"GATE", { 12341.f, 4050.f, -2.f }, { 10.f, 100.f, 1.f },
		{ 0.f, 0.f, 0.f });

	Ptr<Crawler> crawler0 = SpawnActor<Crawler>(
		"Crawler 2", { 5235.f, 3843.f, 0.f }, { 1.f, 1.f, 1.f }, 
		{ 0.f, 0.f ,0.f });

	Ptr<Crawler> crawler1 = SpawnActor<Crawler>(
		"Crawler 3", { 9805.f, 318.f, 0.f }, { 1.f, 1.f, 1.f },
		{ 0.f, 0.f ,0.f });

	Ptr<Crawler> crawler2 = SpawnActor<Crawler>(
		"Crawler 1", { 8032.f, 710.f, 0.f }, { 1.f, 1.f, 1.f },
		{ 0.f, 0.f ,0.f });

	//Ptr<Buzzer> Buzzer1 = SpawnActor<Buzzer>(
	//	"Buzzer 1", { 3852.f, 2524.f, 0.f }, { 1.f, 1.f, 1.f },
	//	{ 0.f, 0.f ,0.f });

	//Ptr<Buzzer> Buzzer2 = SpawnActor<Buzzer>(
	//	"Buzzer 2", { 5547.f, 2071.f, 0.f }, { 1.f, 1.f, 1.f },
	//	{ 0.f, 0.f ,0.f });

	//Ptr<Buzzer> Buzzer3 = SpawnActor<Buzzer>(
	//	"Buzzer 3", { 5343.f, 3091.f, 0.f }, { 1.f, 1.f, 1.f },
	//	{ 0.f, 0.f ,0.f });

	gateActor->SetLevelType(eLevelType::TOWN);
	gateActor->SetGateType(GATETYPE::IMMEDIATE);

	Ptr<ParticleComponent> smoke = smokeActor->CreateSceneComponent<ParticleComponent>("SmokeParticle");
	smoke->SetEmitProperty(MakeWhitePalacePreset());
	smoke->SetTexture("light_effect_v02");
	smokeActor->SetRootComponent(smoke);

	LoadProbInstance(L"Tutorial_01_StaticProbInstances.json");
	LoadLightObject(L"Tutorial_01_LightMaskObjects.json");
	LoadLineColliders(L"Tutorial_01_SceneChunkMap.json");
	LoadColliderObject(L"Tutorial_01_SceneColliderObjects.json");
	LoadCameraLock(L"Tutorial_01_SceneCameraLockZones.json");
	//LoadAnimationActor(L"Tutorial_01_AnimationObjects.json");

	FLocalBound2D lockBound = GetLevelBound();
	GetMainCamera()->SetWorldLockBound(lockBound);
	GetMainCamera()->SetCameraHalf(_cameraHalfData);

	return true;
}

void TutorialScene::Destroy()
{
	Level::Destroy();
}

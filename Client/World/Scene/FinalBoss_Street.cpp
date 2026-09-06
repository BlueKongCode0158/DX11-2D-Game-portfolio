#include "pch.h"
#include "FinalBoss_Street.h"

#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"

FinalBoss_Street::FinalBoss_Street()
{
}

FinalBoss_Street::~FinalBoss_Street()
{
}

bool FinalBoss_Street::Init(const std::string& name)
{
	Level::Init(name);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(3300.f, 920.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(1.f, 1.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 10.7f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);


	LoadProbInstance(L"Room_Final_Boss_Core_StaticProbInstances.json");
	LoadLightObject(L"Room_Final_Boss_Core_SceneLightMaskObjects.json");
	LoadLineColliders(L"Room_Final_Boss_Core_SceneChunkMap.json");
	//LoadAnimationActor(L"Room_Final_Boss_Core_AnimationObjects.json");

	return true;
}

void FinalBoss_Street::Destroy()
{
	Level::Destroy();
}

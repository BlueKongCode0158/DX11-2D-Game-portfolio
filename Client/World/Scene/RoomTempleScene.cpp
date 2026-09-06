#include "pch.h"
#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"
#include "RoomTempleScene.h"

RoomTempleScene::RoomTempleScene()
{
}

RoomTempleScene::~RoomTempleScene()
{
}

bool RoomTempleScene::Init(const std::string& name)
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

	LoadProbInstance(L"GG_Hornet_1_StaticProbInstances.json");
	LoadLightObject(L"GG_Hornet_1_LightMaskObjects.json");
	LoadAnimationActor(L"GG_Hornet_1_AnimationObjects.json");
	LoadLineColliders(L"GG_Hornet_1_SceneChunkMap.json");

	return true;
}

void RoomTempleScene::Destroy()
{
	Level::Destroy();
}

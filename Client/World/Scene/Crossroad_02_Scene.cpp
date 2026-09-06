#include "pch.h"
#include "Crossroad_02_Scene.h"

#include "../../Object/Player.h"
#include "../../Object/FogPlane.h"

Crossroad_02::Crossroad_02()
{
}

Crossroad_02::~Crossroad_02()
{
}

bool Crossroad_02::Init(const std::string& path)
{
	Level::Init(path);

	FVector3D scale = FVector3D(1.f, 1.f, 1.f);
	FVector3D pos = FVector3D(3300.f, 920.f, 0.f);
	FRotator rot = FRotator(0.f, 0.f, 0.f);
	Ptr<Player> t1 = SpawnActor<Player>("Knight", pos, scale, rot);
	t1->AddTags("Player", "Human");

	FVector3D fogscale = FVector3D(100.f, 100.f, 1.f);
	FVector3D fogPos = FVector3D(2000.f, 800.f, 6.62f);
	Ptr<FogPlane> t2 = SpawnActor<FogPlane>("FOG", fogPos, fogscale, rot);
	LoadProbInstance(L"Crossroads_02_StaticProbInstances.json");
	LoadLightObject(L"Crossroads_02_SceneLightMaskObjects.json");
	LoadLineColliders(L"Crossroads_02_SceneChunkMap.json");
	//LoadAnimationActor(L"Crossroads_01_AnimationObjects.json");
	return true;
}

void Crossroad_02::Destroy()
{
	Level::Destroy();
}

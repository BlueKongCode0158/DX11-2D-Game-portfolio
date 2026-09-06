#include "pch.h"
#include "World.h"
#include "Level.h"
#include "Scene/TownScene.h"
#include "Scene/TutorialScene.h"
#include "Scene/Crossroad_01_Scene.h"
#include "Scene/Crossroad_02_Scene.h"
#include "Scene/Crossroad_10.h"
#include "Scene/Crossroad_11_Scene.h"
#include "Scene/FinalBoss_Street.h"
#include "Scene/RoomTempleScene.h"
#include "Scene/Grimm_MainTent.h"
#include "Scene/MainMenu.h"

#include "../Object/Camera.h"
#include "../World/CameraManager.h"

#include "../Render/RenderManager.h"

World::World()
{
}

World::~World()
{
}

void World::Init(const std::string& name)
{
	// TODO save And load7

	_cameraManager = New<CameraManager>();

	_mainCam = New<Camera>();
	_curLevel = CreateLevel<TutorialScene>(name);
	_curLevelType = eLevelType::TUTORIAL;

	_nextLevelType = _curLevelType;

	_mainCam->SetLevel(_curLevel);
	// id는 독자적인 id니까 world 상의 액터는 음수의 아이디를 가지게 하자.
	// 우선 카메라는 -1000으로 설정해둘 것.
	_mainCam->Init(-1000, FVector3D(0.f, 0.f, 0.f), FVector3D(1.f, 1.f, 1.f), FRotator(0.f, 0.f, 0.f), "mainCamera");

	_cameraManager->Init(_curLevel);
	return;
}

void World::Tick(float deltaTime)
{
	if (_curLevelType != _nextLevelType)
	{
		DESTROY(_curLevel);
		RenderManager::Instance().FlushRemovals();

		ChanageLevel(_nextLevelType);

		_curLevelType = _nextLevelType;
		_curLevel = _nextLevel;
		_nextLevel = nullptr;
		_cameraManager->SetCurLevel(_curLevel);
	}

	if (_curLevel)
	{
		_curLevel->Tick(deltaTime);
	}
}

void World::Collision(float deltaTime)
{
	if (_curLevel)
	{
		_curLevel->Collision(deltaTime);
	}
}

void World::Render(float deltaTime)
{
	if (_curLevel)
	{
		_curLevel->Render(deltaTime);
	}
}

void World::RenderUI(float deltaTime)
{
	if (_curLevel)
	{
		_curLevel->RenderUI(deltaTime);
	}
}

Ptr<class CameraManager> World::GetCamaeraManager()
{
	return _cameraManager;
}

void World::SetNextLevelType(const eLevelType levelType)
{
	_nextLevelType = levelType;
}

void World::ChanageLevel(const eLevelType levelType, const std::string path /*= ""*/)
{
	switch (levelType)
	{
	case MAIN:
		_nextLevel = CreateLevel<MainMenuScene>(path);
		break;
	case TUTORIAL:
		_nextLevel = CreateLevel<TutorialScene>(path);
		break;
	case TOWN:
		_nextLevel = CreateLevel<TownScene>(path);
		break;
	case CROSSROAD_01:
		_nextLevel = CreateLevel<Crossroad_01>(path);
		break;
	case CROSSROAD_02:
		_nextLevel = CreateLevel<Crossroad_02>(path);
		break;
	case CROSSROAD_10:
		_nextLevel = CreateLevel<Crossrad_10_Scene>(path);
		break;
	case CROSSROAD_11:
		_nextLevel = CreateLevel<Crossroad_11>(path);
		break;
	case TEMPLE:
		_nextLevel = CreateLevel<RoomTempleScene>(path);
		break;
	case TEMPLESTREET:
		_nextLevel = CreateLevel<FinalBoss_Street>(path);
		break;
	case GRIMMTENT:
		_nextLevel = CreateLevel<Grimm_MainTent>(path);
		break;
	case LEVEL_END:
		break;
	default:
		break;
	}
}

void World::Destroy()
{
	DESTROY(_curLevel);
	DESTROY(_nextLevel);
	DESTROY(_mainCam);
	DESTROY(_cameraManager);
}

Ptr<class Level> World::GetCurLevel() const
{
	return _curLevel;
}

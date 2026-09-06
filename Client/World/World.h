#pragma once
#include "Object/Object.h"

class World : public Object
{
public:
	World();
	virtual ~World();
	World(const World& other) = delete;
	World(World&& other) = delete;
	World& operator =(const World& other) = delete;
	World& operator =(World&& other) = delete;
public:
	virtual void Init(const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void RenderUI(float deltaTime);
public:
	Ptr<class CameraManager> GetCamaeraManager();
public:
	void SetNextLevelType(const eLevelType levelType);
	void ChanageLevel(const eLevelType levelType, const std::string path = "");
public:
	virtual void Destroy() override;
	Ptr<class Level> GetCurLevel() const;
public:
	template<typename T>
	Ptr<T> CreateLevel(const std::string& path)
	{
		Ptr<T> level = New<T>();
		if (false == level->Init(path))
		{
			DESTROY(level);
			return nullptr;
		}
		return level;
	}
private:
	Ptr<class Level> _curLevel;
	Ptr<class Level> _nextLevel;
	Ptr<class Camera> _mainCam;
private:
	Ptr<class CameraManager> _cameraManager;
private:
	eLevelType _curLevelType	= eLevelType::LEVEL_END;
	eLevelType _nextLevelType	= eLevelType::LEVEL_END;
};
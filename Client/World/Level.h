#pragma once
#include "Object/Object.h"
#include "Object/Actor.h"
#include "../Editor/Interface/Interface.h"

class Level : public Object, public IArchive, public IComponentOwner
{
public:
	Level();
	virtual ~Level();
	Level(const Level& other) = delete;
	Level(Level&& other) = delete;
	Level& operator = (const Level& other) = delete;
	Level& operator = (Level&& other) = delete;
private:
	int32 _actorID = 0;
	std::map<int32, Ptr<class Actor>>	_actors;
	std::vector<int32>					_removeActors;
	Ptr<class TagManager>				_tagManger;
	Ptr<class CollisionManager>			_collisionManager;
	Ptr<class UIManager>				_uiManager;
private:
	Weak<class CameraManager>			_cameraManager;
	Weak<class Actor>					_Player;
private:
	int32 _levelComponentID = 0;
	std::map<int32, Ptr<class Component>>	_levelComponents;
	std::unordered_map<std::string, int32>	_levelComponentsFinders;

	// 환경 정적 엣지 콜라이더 전용 저장소. 이름으로 조회되지 않고 매 프레임 Tick 도 돌지
	// 않으므로 _levelComponents/_levelComponentsFinders 경로를 타지 않는다.
	std::vector<Ptr<class EdgeCollisionComponent>> _staticEdgeColliders;
protected:
	FLocalBound2D	_worldBound = FLocalBound2D();
	FVector2D		_cameraHalfData;
public:
	virtual bool Init(const std::string& path);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void RenderUI(float deltaTime);
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	virtual Ptr<Level>  GetLevel() override;
	virtual int32		GetOwnerID() override;
public:
	Ptr<class Actor>	GetPlayer()const;
	void				SetPlayer(Ptr<class Actor> actor);
public:
	Ptr<class CameraManager> GetCameraManager();
public:
	void				SetLevelBound(FLocalBound2D localBound);
	FLocalBound2D		GetLevelBound() const;
public:
	void AddTag(const std::string& tag, int32 id);
	void DeleteTag(Ptr<class Actor> actor);
	Ptr<class Actor> FindActor(int32 id);
	void FindActors(const std::string& tag, OUT std::vector<Ptr<class Actor>>& outArr);
	void RemoveActor(int32 id);

	void SetMainCamera(Ptr<class CameraComponent> camera);
	Ptr<class CameraComponent> GetMainCamera() const;
	const FMatrix& GetWorldMatrix();
	const FMatrix& GetViewMatrix();
	const FMatrix& GetProjMatrix();
	const FMatrix& GetUIProjMatrix();
	const FVector3D& GetCameraWorldPos();
	
	void AddCollision(std::pair<int32, int32>& colID, Ptr<class CollisionComponent> comp);
	Ptr<class CollisionComponent> FindCollider(std::pair<int32, int32>& colID);
	void RemoveCollision(std::pair<int32, int32>& colID);
	const std::map<int32, Ptr<class Actor>>& GetActors();
	const std::map<int32, Ptr<class Component>>& GetLevelComponents() const { return _levelComponents; }
protected:
	bool LoadProbInstance(const std::wstring& fileName);
	bool LoadLineColliders(const std::wstring& fileName);
	bool LoadLightObject(const std::wstring& fileName);
	bool LoadAnimationActor(const std::wstring& fileName);
	bool LoadColliderObject(const std::wstring& fileName);
	bool LoadCameraLock(const std::wstring& fileName);
	Ptr<class EdgeCollisionComponent> AddEdgeCollider(const std::string& name, const FVector2D& worldStart,
		const FVector2D& worldEnd);
public:
	template<typename T>
	Ptr<T> FindActor(int32 id)
	{
		auto it = _actors.find(id);
		if (_actors.end() == it)
		{
			return nullptr;
		}
		return Cast<Actor, T>(it->second);
	}

	template<typename T>
	Ptr<T> SpawnActor(const std::string& name, const FVector3D& pos, const FVector3D& scale, const FVector3D& rot)
	{
		Ptr<T> actor = New<T>();

		actor->SetLevel(This<Level>());
		const int actorID = _actorID++;
		if (false == actor->Init(actorID, pos, scale, rot, name))
		{
			DESTROY(actor);
			return nullptr;
		}
		_actors[actorID] = actor;
		return actor;
	}

	template<typename T>
	Ptr<T> CreateLevelComponent(const std::string& name)
	{
		Ptr<T> comp = New<T>();
		int32 levelCompID = _levelComponentID++;
		if (!comp->Init(levelCompID, name, This<Level>()))
		{
			DESTROY(comp);
			return nullptr;
		}
		_levelComponentsFinders[name] = levelCompID;
		_levelComponents[levelCompID] = comp;
		return comp;
	}

	template<typename T>
	Ptr<T> FindLevelComponent(const std::string& name)
	{
		auto it = _levelComponentsFinders.find(name);
		if (_levelComponentsFinders.end() == it)
		{
			return nullptr;
		}
		auto comp = _levelComponents.find(it->second);
		if (_levelComponents.end() == comp)
		{
			return nullptr;
		}

		return Cast<Component, T>(comp->second);
	}
};

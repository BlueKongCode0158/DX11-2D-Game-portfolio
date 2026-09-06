#pragma once
#include "Object/Object.h"
#include "../Editor/Interface/Interface.h"
#include <set>

class Actor : public Object, public IArchive, public IComponentOwner
{
	friend class Level;
public:
	Actor();
	virtual ~Actor();
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual	void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
public:
	virtual void DrawInspector() override;
public:
	virtual void SetEnable(bool enable);
public:
	Ptr<class SceneComponent> GetRoot() const;
	void SetRootComponent(const Ptr<class SceneComponent> comp);
	Ptr<class SceneComponent> FindSceneComponent(const std::string& name) const;
	Ptr<class ActorComponent> FindActorComponent(const std::string& name) const;
	Ptr<class SceneComponent> FindSceneComponent(int32 id) const;
	Ptr<class ActorComponent> FindActorComponent(int32 id) const;
public:
	Ptr<class Level> GetLevel()		override;
	int32			 GetOwnerID()	override;
	void SetLevel(Ptr<class Level> level);
	void SetName(const std::string& name);
	void Remove();
public:
	const int32 GetActorID() const { return _id; }
	const std::string& GetName() const;
	bool IsTag(const std::string& tag);
	void AddTag(const std::string& tag);
	eActorType GetType() const;
	
	template<typename...Args>
	void AddTags(Args&&...args)
	{
		(AddTag(std::forward<Args>(args)), ...);
	}
public:
	// GET
	const FTransform& GetWorldTransform() const;
	const FVector3D& GetWorldScale() const;
	const FVector3D& GetWorldRotation() const;
	const FVector3D& GetWorldPosition() const;

	// SET
	void SetWorldTransform(const FTransform& tran);
	void SetWorldTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot);

	void SetWorldScale(const FVector3D& scale);
	void SetWorldScale(const float x, const float y, const float z);
	void SetWorldScale(const FVector2D& scale);
	void SetWorldScale(const float x, const float y);

	void SetWorldPosition(const FVector3D& pos);
	void SetWorldPosition(const FVector2D& pos);
	void SetWorldPosition(float x, float y);
	void SetWorldPosition(float x, float y, float z);


	// 부동 소수점 오차가 누적되어 값의 오차가 심해짐 -> Add 생략
	void SetWorldRotation(const FRotator& rot);
	void SetWorldRotation(const FVector2D& rot);
	void SetWorldRotation(const float x, const float y, const float z);
	void SetWorldRotation(const float x, const float y);
	
	// ADD
	const FVector3D& AddWorldScale(const FVector3D& scale);
	const FVector3D& AddWorldScale(const FVector2D& scale);
	const FVector3D& AddWorldScale(const float x, const float y, const float z);
	const FVector3D& AddWorldScale(const float x, const float y);
	const FVector3D& AddWorldScale(const float value);
	const FVector3D& AddWorldScaleX(float x);
	const FVector3D& AddWorldScaleY(float y);

	const FVector3D& AddWorldPosition(const FVector3D& pos);
	const FVector3D& AddWorldPosition(const FVector2D& pos);
	const FVector3D& AddWorldPosition(const float x, const float y);
	const FVector3D& AddWorldPosition(const float x, const float y, const float z);
	const FVector3D& AddWorldPosition(const float value);
	const FVector3D& AddWorldPositionX(const float x);
	const FVector3D& AddWorldPositionY(const float y);
public:
	// GET
	const FVector3D& GetRelativeScale() const;
	const FVector3D& GetRelativeRotation() const;
	const FVector3D& GetRelativePosition() const;
	const FTransform& GetRelativeTransform() const;
	// SET
	void SetRelativeTransform(const FTransform& tran);
	void SetRelativeTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot);

	void SetRelativeScale(const FVector3D& scale);
	void SetRelativeScale(const FVector2D& scale);
	void SetRelativeScale(const float x, const float y, const float z);
	void SetRelativeScale(const float x, const float y);

	void SetRelativeRotation(const FRotator& rot);
	void SetRelativeRotation(const FVector2D& rot);
	void SetRelativeRotation(const float x, const float y, const float z);
	void SetRelativeRotation(const float x, const float y);

	void SetRelativePosition(const FVector3D& pos);
	void SetRelativePosition(const FVector2D& pos);
	void SetRelativePosition(const float x, const float y, const float z);
	void SetRelativePosition(const float x, const float y);

	// ADD
	const FVector3D& AddRelativeScale(const FVector3D& scale);
	const FVector3D& AddRelativeScale(const FVector2D& scale);
	const FVector3D& AddRelativeScale(const float x, const float y, const float z);
	const FVector3D& AddRelativeScale(const float x, const float y);
	const FVector3D& AddRelativeScale(const float value);
	const FVector3D& AddRelativeScaleX(const float x);
	const FVector3D& AddRelativeScaleY(const float y);

	const FVector3D& AddRelativePosition(const FVector3D& pos);
	const FVector3D& AddRelativePosition(const FVector2D& pos);
	const FVector3D& AddRelativePosition(const float x, const float y);
	const FVector3D& AddRelativePosition(const float x, const float y, const float z);
	const FVector3D& AddRelativePosition(const float value);
	const FVector3D& AddRelativePositionX(const float x);
	const FVector3D& AddRelativePositionY(const float y);

	const FVector3D& AddRelativeRotation(const FRotator& rot);
	const FVector3D& AddRelativeRotation(const FVector2D& pos);
	const FVector3D& AddRelativeRotation(const float x, const float y);
	const FVector3D& AddRelativeRotation(const float x, const float y, const float z);
	const FVector3D& AddRelativeRotation(const float value);
	const FVector3D& AddRelativeRotationX(const float x);
	const FVector3D& AddRelativeRotationY(const float y);
public:
	// Object을(를) 통해 상속됨
	void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	template<typename T>
	Ptr<T> FindComponent(const std::string& name) const
	{
		Ptr<SceneComponent> foundComp = FindSceneComponent(name);
		if (nullptr != foundComp)
		{
			return Cast<SceneComponent, T>(foundComp);
		}
		Ptr<ActorComponent> foundActorComp = FindActorComponent(name);
		if (nullptr != foundActorComp)
		{
			return Cast<ActorComponent, T>(foundActorComp);
		}
		return nullptr;
	}

	template<typename T>
	Ptr<T> FindSceneComponent(const std::string& name) const
	{
		Ptr<SceneComponent> foundComp = FindSceneComponent(name);
		if (nullptr != foundComp)
		{
			return Cast<SceneComponent, T>(foundComp);
		}
		return nullptr;
	}

	template<typename T>
	Ptr<T> FindActorComponent(const std::string& name) const
	{
		Ptr<ActorComponent> foundActorComp = FindActorComponent(name);
		if (nullptr != foundActorComp)
		{
			return Cast<ActorComponent, T>(foundActorComp);
		}
		return nullptr;
	}
public:
	template<typename T>
	Ptr<T> CreateSceneComponent(const std::string& name)
	{
		Ptr<T> comp = New<T>();
		if(false == comp->Init(_componentID, name, This<Actor>()))
		{
			comp->Destroy();
			Delete(comp);
		}
		_componentFinder[name] = _componentID;
		_componentID++;
		return comp;
	}

	template<typename T>
	Ptr<T> CreateActorComponent(const std::string& name)
	{
		Ptr<T> comp = New<T>();
		if (false == comp->Init(_componentID, name, This<Actor>()))
		{
			comp->Destroy();
			Delete(comp);
		}
		_componentFinder[name] = _componentID;
		_actorComponents[_componentID] = comp;
		_componentID++;

		return comp;
	}
protected:
	std::string _name;
	int32 _componentID = 0;
	int32 _id = -1;
	Ptr<class SceneComponent> _root;
	Weak<class Level> _level;
	std::map<int, Ptr<class ActorComponent>> _actorComponents;
	std::unordered_map<std::string, int> _componentFinder;
	eActorType _type = eActorType::Actor;
	std::set<std::string> _tags;
};


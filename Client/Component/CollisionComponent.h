#pragma once
#include "SceneComponent.h"
#include <functional>

class CollisionComponent : public SceneComponent
{
	friend class CollisionManager;
public:
	CollisionComponent();
	virtual ~CollisionComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	virtual bool Collision(Weak<CollisionComponent> dest);
	void SetCollisionProfile(const std::string& name);
	const Ptr<class CollisionProfile> GetProfile() const;
	// 정적(움직이지 않는 환경) 콜라이더 여부. CollisionManager 가 static/dynamic
	// 버킷 분기에 사용한다. 등록(Insert)은 base Init 에서 일어나므로, 정적으로 쓸
	// 콜라이더는 생성자에서 _isStatic 을 켜 둬야 한다.
	bool IsStatic() const { return _isStatic; }
	void SetStatic(bool isStatic) { _isStatic = isStatic; }
protected:
	bool _isStatic = false;
	eCollisonShape _shape = eCollisonShape::END;
	FVector3D _min;
	FVector3D _max;

	std::pair<int32, int32> _colliderID;
	Ptr<class CollisionProfile> _profile;
	std::function<void(Weak<CollisionComponent>)> _collisionCallBack[eCollisionState::END];
	std::map<std::pair<int32, int32>, eCollisionState> _colliderReations;
#if _DEBUG
	// todo : debug collision
	Ptr<class TransformCBuffer> _transformCBuffer = nullptr;
	Ptr<class ColorCBuffer> _colorCBuffer = nullptr;
	Ptr<class Mesh> _mesh = nullptr;
	Ptr<class Shader> _shader = nullptr;
#endif
public:
	eCollisonShape GetShape() const { return _shape; }
	eCollisionState CheckState(std::pair<int32, int32>& destKey);
	const std::pair<int32, int32>& GetColliderID() const;
private:
	void Invoke(eCollisionState state, Weak<CollisionComponent> dest, const std::pair<int32, int32>& destKey);
public:
	template<typename T>
	void SetCollisionCallBack(eCollisionState state, T* obj, void (T::* memfunc)(Weak<CollisionComponent>))
	{
		_collisionCallBack[state] = std::bind(memfunc, obj, std::placeholders::_1);
	}
	template<typename T>
	void SetCollisionCallBack(eCollisionState state, T&& func)
	{
		_collisionCallBack[state] = std::forward<T>(func);
	}
};
#pragma once
#include "Component.h"

// 트랜스폼(위치, 회전, 크기) 정보를 가지고 있는 컴포넌트
// 트랜스폼을 가져야 하는 모든 컴포넌트의 부모 클래스
// 씬 컴포넌트는 다른 씬컴포넌트와 부모 자식 관계를 이룰 수 있다.
// 자식 씬 컴포넌트는 부모 컴포넌트로부터 위치, 크기, 회전을 영향 받는다.

class SceneComponent : public Component
{
	friend class Actor;
public:
	SceneComponent();
	virtual ~SceneComponent();
public:
	const Ptr<SceneComponent> GetParent() const;
	void AttachToComponent(Ptr<SceneComponent> comp);	// 부모 붙이는건가?
	void AddChild(Ptr<SceneComponent> comp);
	Ptr<SceneComponent> FindComponent(int32 id);
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
	virtual void SetEnable(bool enable) override;
public:
	const std::map<int32, Ptr<SceneComponent>>& GetChilds() const;
public:
	// World Transform
	const FTransform& GetWorldTransform() const;
	void SetWorldTransform(const FTransform& transform);
	void SetWorldTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot);

	// World Scale
	const FVector3D& GetWorldScale() const;
	void SetWorldScale(const FVector3D& scale);
	void SetWorldScale(float x, float y, float z);
	void SetWorldScale(const FVector2D& scale);
	void SetWorldScale(float x, float y);
	const FVector3D& AddWorldScale(const FVector3D& scale);
	const FVector3D& AddWorldScale(const FVector2D& scale);
	const FVector3D& AddWorldScale(const float x, const float y, const float z);
	const FVector3D& AddWorldScale(const float x, const float y);
	const FVector3D& AddWorldScale(const float value);
	const FVector3D& AddWorldScaleX(const float x); 
	const FVector3D& AddWorldScaleY(const float y);

	// World Position
	const FVector3D& GetWorldPosition() const;
	void SetWorldPosition(const FVector3D& pos);
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(const FVector2D& pos);
	void SetWorldPosition(float x, float y);
	const FVector3D& AddWorldPosition(const FVector3D& pos);
	const FVector3D& AddWorldPosition(const FVector2D& pos);
	const FVector3D& AddWorldPosition(const float x, const float y, const float z);
	const FVector3D& AddWorldPosition(const float x, const float y);
	const FVector3D& AddWorldPosition(const float value);
	const FVector3D& AddWorldPositionX(const float x);
	const FVector3D& AddWorldPositionY(const float y);

	// World Rotation
	const FRotator& GetWorldRotation() const;
	void SetWorldRotation(const FRotator& rot);
	void SetWorldRotation(float x, float y, float z);
	void SetWorldRotation(const FVector2D& rot);
	void SetWorldRotation(float x, float y);

	// Relative Transform 
	const FTransform& GetRelativeTransform() const;
	void SetRelativeTransform(const FTransform& transform);
	void SetRelativeTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot);

	// Relative Scale
	const FVector3D& GetRelativeScale() const;
	void SetRelativeScale(const FVector3D& scale);
	void SetRelativeScale(float x, float y, float z);
	void SetRelativeScale(const FVector2D& scale);
	void SetRelativeScale(float x, float y);
	const FVector3D& AddRelativeScale(const FVector3D& scale);
	const FVector3D& AddRelativeScale(const FVector2D& scale);
	const FVector3D& AddRelativeScale(const float x, const float y, const float z);
	const FVector3D& AddRelativeScale(const float x, const float y);
	const FVector3D& AddRelativeScale(const float value);
	const FVector3D& AddRelativeScaleX(const float x);
	const FVector3D& AddRelativeScaleY(const float y);

	// Relative Position
	const FVector3D& GetRelativePosition() const;
	void SetRelativePosition(const FVector3D& pos);
	void SetRelativePosition(float x, float y, float z);
	void SetRelativePosition(const FVector2D& pos);
	void SetRelativePosition(float x, float y);
	const FVector3D& AddRelativePosition(const FVector3D& pos);
	const FVector3D& AddRelativePosition(const FVector2D& pos);
	const FVector3D& AddRelativePosition(const float x, const float y, const float z);
	const FVector3D& AddRelativePosition(const float x, const float y);
	const FVector3D& AddRelativePosition(const float value);
	const FVector3D& AddRelativePositionX(const float x);
	const FVector3D& AddRelativePositionY(const float y);

	// Relative Rotation
	const FRotator& GetRelativeRotator() const;
	void SetRelativeRotation(const FRotator& rot);
	void SetRelativeRotation(float x, float y, float z);
	void SetRelativeRotation(const FVector2D& rot);
	void SetRelativeRotation(float x, float y);
	const FRotator& AddRelativeRotation(const FRotator& rot);
	const FRotator& AddRelativeRotation(const FVector2D& rot);
	const FRotator& AddRelativeRotation(const float x, float y, float z);
	const FRotator& AddRelativeRotation(const float x, float y);
	const FRotator& AddRelativeRotation(const float value);
	const FRotator& AddRelativeRotationX(const float x);
	const FRotator& AddRelativeRotationY(const float y);

	// BlendMode
	const eBlendMode GetBlendMode() { return _eBlendState; }
	void SetBlendMode(eBlendMode eMode)
	{
		_eBlendState = eMode;
	}
public:
	const FMatrix& GetWorldMatrix() const
	{
		return _matrix._world;
	}
public:
	bool ConsumeRenderBoundDirty();
	virtual bool BuildRenderBound2D(OUT FLocalBound2D& out) const;
protected:
	bool BuildBoundFromLocal(const FLocalBound2D& local, OUT FLocalBound2D& out) const;
	void MarkRenderBoundDirty();
public:
	const std::string& GetRenderLayerName() { return _renderLayerName; }
	void SetRenderLayerName(const std::string& name);
protected:
	void UpdateTransform();
protected:
	Ptr<class TransformCBuffer> _transformCBuffer;
	Weak<SceneComponent> _parent;					// 부모 컴포넌트
	std::map<int32, Ptr<SceneComponent>> _childs;	// 자식 컴포넌트 목록
	std::string _renderLayerName;
	bool _isRender; // 그려야하는 scenecomponent를 구별하기 위함.
	FTransform _world;
	FTransform _relative;
	FTransformMatirx _matrix;
	FVector3D _axis[AXIS_TYPE::END] = {
		FVector3D(FVector3D::Axis_X),
		FVector3D(FVector3D::Axis_Y),
		FVector3D(FVector3D::Axis_Z) 
	};

	eBlendMode	_eBlendState	  = eBlendMode::DEFAULT;
	bool		_renderBoundDirty = true;
private:
	bool		_transformDirty = false;
};


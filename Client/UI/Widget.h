#pragma once
#include "../Object/Object.h"
#include "../Editor/Interface/Interface.h"

struct FUIBrush
{
	Ptr<class Texture> _texture;
	FVector4D _tint = FVector4D(1.f, 1.f, 1.f, 1.f);
	std::vector<FAnimationFrame> _frames;
	Ptr<class Animation2DSequence> _sequence;
	float _playRate = 1.f;
	bool _animEnable = false;
	void SetSequence(const std::string& dataName, bool loop, bool reverse, float playTime, float playRate, float loopFrame);
	void Tick(float deltaTime);
	void SetShader(Ptr<class UICBuffer> cbuffer);
};

class Widget : public Object, public IComponentOwner
{
public:
	Widget();
	virtual ~Widget();
protected:
	int32 _widgetID = -1;
	std::string _name;

	Weak<class Level> _level;
	Weak<class Actor> _ownerActor;

	Weak<Widget> _parent;
	std::vector<Ptr<Widget>> _children;
	std::vector<int32> _removeWidgets;

	Ptr<class Shader> _shader;
	Ptr<class Mesh> _mesh;
	Ptr<class TransformCBuffer> _transformCBuffer;
	Ptr<class UICBuffer> _uiCBuffer;

	FVector2D	_pos;
	FVector2D	_renderPos;
	FVector2D	_releativeSize = { 1.f, 1.f };
	FVector2D	_size;
	int16		_order;
	bool		_sorted = false;
public:
	virtual Ptr<class Level> GetLevel();
	virtual int32			 GetOwnerID();

	void SetOwnerActor(Weak<class Actor> owner);
	Ptr<class Actor> GetOwnerActor() const;
	
	void SetParent(Ptr<Widget> parent);
	Ptr<Widget> GetParent() const;

	const FVector2D& GetPos() const;
	const FVector2D& GetRenderPos() const;
	const FVector2D& GetSize() const;
	int16 GetOrder() const;
	void SetOrder(int16 order);
	void SetRenderPos(const FVector2D& renderPos);

	virtual void SetPos(const FVector2D& pos);
	virtual void SetSize(const FVector2D& size);

	void RemoveChild(int32 childID);
	void AddChild(Ptr<Widget> child);
	void SetSorted(bool sort);
public:
	virtual bool Init(int32 id, const std::string& name, Weak<class Level> level);
	virtual void Tick(float deltaTime);
	virtual void Render();
	virtual void Render(const FVector3D& pos); // 액터에게 붙어야할 때
public:
	virtual Ptr<Widget> MouseHit(const FVector2D& pos);
	virtual bool IsInteractable();
	virtual void OnHovered();
	virtual void OnUnHovered();
	virtual void OnClick();
	virtual void OnPressed();
	virtual void Destroy();
public:
	virtual void SetAnimationPlayTime(float time);
	virtual void SetAnimationPlayRate(float rate);
public:
	virtual void SetBrushAnimation(const std::string& name);
	virtual void SetBrushAnimationPlay(bool play);
	virtual void SetBrushAnimationLoop(bool loop);
public:
	bool IsInSide(const FVector2D& pos);
};
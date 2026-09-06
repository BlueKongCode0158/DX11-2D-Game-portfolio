#pragma once
#include "../SceneComponent.h"
#include "Animation2D.h"
#include "../RenderModifier2DComponent.h"

// 그림을 그리는 컴포넌트

class SpriteComponent : public SceneComponent
{
public:
	SpriteComponent();
	virtual ~SpriteComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
	virtual bool BuildRenderBound2D(OUT FLocalBound2D& out) const override;
protected:
	Ptr<class Mesh> _mesh;
	Ptr<class Shader> _shader;
	Ptr<class Texture> _texture;
	Ptr<class Animation2D> _animation;
	Ptr<class SpriteCBuffer> _spriteCBuffer;
	int32 _textureIndex = 0;
	FVector4D _tint = FVector4D(1.f, 1.f, 1.f, 1.f);
	FVector3D _animationPivot = FVector3D(0.f, 0.f, 0.f);
	std::string _shaderName;
private:
	bool _useAnimationFrameTransform = false;
protected:
	Weak<class RenderModifier2DComponent> _renderModifier;
public:
	void SetRenderModifier(Ptr<class RenderModifier2DComponent> modifier);
	FRenderModifier2D BuildRenderModifier();
public:
	// State Setting
	void SetMesh(const std::string& AtlasName, int32 pathID);
	void SetShader(const std::string& name);
	const std::string& GetShaderName() const;
	void SetTint(const FVector4D& tint);
	void SetTint(float x, float y, float z, float w);
	void SetOpacity(float op);
	// Texture Setting
	void SetTexture(const std::string& name, int textureIndex = 0);
	void SetTexture(Ptr<class Texture> texture, int textureIndex = 0);
	void SetTextureIndex(int32 index);
	// Animation State
	void AddAnimSequence(const std::string& name, bool loop = false, bool reverse = false, float playTime = 1.f, float playRate = 1.f, float loopFrame = 0.f);
	void AddAnimSequence(const std::string& sequenceName, const std::string& dataName, int32 startFrame, int32 frameCount, bool loop = false, bool reverse = false, float playTime = 1.f, float playRate = 1.f, float loopFrame = 0.f);
	void AddAnimSequence(Ptr<class Animation2DData>& data, bool loop = false, bool reverse = false, float playTime = 1.f, float playRate = 1.f, float loopFrame = 0.f);
	// 원본 클립에서 baking 된 fps/loop/loopFrame 을 자동 적용 (수동 playTime 계산 불필요)
	void AddAnimSequenceAuto(const std::string& name, bool reverse = false, float playRate = 1.f);
	void SetLoopAnimationFrame(const std::string& name, int32 loopFrame);
	void SetPlayTime(const std::string& name, float time);
	void SetPlayRate(const std::string& name, float rate);
	void SetLoop(const std::string& name, bool loop);
	void SetReverse(const std::string& name, bool reverse);
	void ChangeAnimation(const std::string& name);
	void SetPlay(const std::string& name, bool play);
	void SetAnimFlip(bool flip);
	void SetAnimPivot(FVector3D pivot);
	void SetAnimPivot(FVector2D pivot);
	void SetAnimPivot(float x, float y, float z);
	bool GetAnimFlip();
	void AddFrameSize(float deltaTime);
	// Create
	Ptr<class Animation2D> CreateAnimation();
	Ptr<class Animation2D> GetAnimation();
	Ptr<class Mesh> GetMesh();
	// Mesh
	void SetMesh(const std::string& name);
public:
	Ptr<class Texture> GetTexture() const
	{
		return _texture;
	}
	const FVector4D& GetTint()
	{
		return _tint;
	}
	int32 GetTextureIndex() const
	{
		return _textureIndex;
	}
	const FVector3D& GetAnimPivot()
	{
		return _animationPivot;
	}
private:
	void ApplyAnimationFrameTransform();
public:
	void SetUseAnimationFrameTransform(bool enable);
public:
	template<typename T>
	void AddNotify(const std::string& sqName, int32 frame, T* obj, void(T::* memfunc)())
	{
		_animation->AddNotify(sqName, frame, obj, memfunc);
	}
	template<typename T>
	void AddNotify(const std::string& sqName, int32 frame, T&& func)
	{
		_animation->AddNotify(sqName, frame, std::forward<T>(func));
	}
};

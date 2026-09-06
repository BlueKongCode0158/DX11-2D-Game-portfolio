#pragma once
#include "SceneComponent.h"

struct FParticleMinMax
{
	float _min = 0.f;
	float _max = 0.f;

	float Evaluate() const
	{
		if (_min == _max)
		{
			return _min;
		}

		return _min + (_max - _min) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}
};

struct FColorKey
{
	float		_time;
	FVector4D	_color;
};

struct FParticle
{
	FVector3D	_position;
	FVector2D	_velocity;
	float		_lifeTime = 0.f;
	float		_maxLifeTime = 0.f;
	FVector4D	_color;
	float		_size = 0.f;
	float		_sizeStart = 0.f;
	float		_rotation = 0.f;
	bool		_isAlive = false;
	int			_animFrame = 0;
	float		_animTime = 0.f;
};

struct FParticleEmitProperty
{
	int32			_maxCount			= 30;
	float			_spawnRate			= 10.f;
	FParticleMinMax _lifeTime			= { 2.f, 4.f };
	FParticleMinMax _speed				= { 0.f, 1.f };
	FParticleMinMax _size				= { 0.3f, 0.7f };
	float			_sizeEnd			= 0.f;
	FParticleMinMax _rotation			= { 0.f, 6.283f };
	FVector2D		_spawnAreaMin;
	FVector2D		_spawnAreaMax;
	FVector2D		_direction			= { 0.f, 1.f };
	float			_spread				= 0.5f;
	float			_gravity			= 0.f;
	bool			_isLoop				= true;
	float			_duration			= 1.f;
	bool			_moveWithTransform	= false;
	eBlendMode		_blendMode			= eBlendMode::DEFAULT;
	Ptr<class Animation2DData> _animData = nullptr;
	float								_animFPS = 0.f;
	bool								_animLoop = true;
	bool								_animSyncLifetime = false;


	std::vector<FColorKey> _colorKeys = {
		{0.0f, {1.f, 1.f, 1.f, 0.00f}}
	};
};

class ParticleComponent : public SceneComponent
{
public:
	ParticleComponent();
	virtual ~ParticleComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void DrawInspector() override;
	virtual void Destroy() override;
	virtual bool BuildRenderBound2D(OUT FLocalBound2D& out) const override;
public:
	void SetEmitProperty(const FParticleEmitProperty& prop);
	void SetTexture(const std::string& name);
	void SetTexture(Ptr<class Texture> texture);
	void SetAnimation(Ptr<class Animation2DData> animData);
public:
	void Play();
	void Stop();
	bool IsPlaying() const;
public:
	// 에디터 프리뷰처럼 레벨 카메라가 아닌 별도 카메라로 그려야 할 때 사용한다.
	// 오버라이드가 설정되면 Render() 는 레벨 view/proj 대신 이 행렬을 쓴다.
	void SetCameraOverride(const FMatrix& view, const FMatrix& proj)
	{
		_useCameraOverride = true;
		_overrideView = view;
		_overrideProj = proj;
	}
	void ClearCameraOverride()
	{
		_useCameraOverride = false;
	}
public:
	const FParticleEmitProperty& GetEmitProperty() const
	{
		return _emitProperty;
	}
	Ptr<class Texture> GetTexture() const
	{
		return _texture;
	}
private:
	void SpawnOne();
	FVector4D EvaluateColorKeys(float t) const;
private:
	FParticleEmitProperty _emitProperty;
	std::vector<FParticle> _particles;
private:
	Ptr<class Mesh> _mesh;
	Ptr<class  Shader> _shader;
	Ptr<class Texture> _texture;
	Ptr<class TransformCBuffer> _transformCBuffer;
	Ptr<class ParticleCBuffer> _particleCBuffer;
	Ptr<class ParticleSBuffer> _particleSBuffer;
private:
	float _spawnAccum = 0.f;
	float _elapsedTime = 0.f;
	float _aliveCount = 0.f;
	bool _isPlaying = true;
private:
	// 카메라 오버라이드 (에디터 프리뷰 전용). false 면 레벨 카메라를 사용한다.
	bool _useCameraOverride = false;
	FMatrix _overrideView;
	FMatrix _overrideProj;
};

// Inspector / 전용 파티클 에디터 패널에서 공유하는 속성 편집 UI.
// 어느 한 필드라도 변경되면 true 를 반환한다(호출측이 재적용 여부 판단).
bool DrawParticleEmitPropertyUI(FParticleEmitProperty& prop);

// 코드/에디터 양쪽에서 쓰는 공용 프리셋. (정의: ParticleComponent.cpp)
FParticleEmitProperty MakeSmokePreset();
FParticleEmitProperty MakeDustPreset();
FParticleEmitProperty MakeFirePreset();
FParticleEmitProperty MakeWhitePalacePreset();
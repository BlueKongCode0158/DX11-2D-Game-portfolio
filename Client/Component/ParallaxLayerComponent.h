#pragma once
#include "RenderModifier2DComponent.h"

// 패럴렉스 계산을 아는 컴포넌트
class ParallaxLayerComponent : public RenderModifier2DComponent
{
public:
	ParallaxLayerComponent();
	virtual ~ParallaxLayerComponent();
public:
	virtual FRenderModifier2D BuildRenderModifier(Ptr<class SpriteComponent> target);
	static void DrawGlobalInspector();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetTarget(Ptr<class SpriteComponent> target);
	Ptr<SpriteComponent> GetTarget();
	FVector2D BuildBottomPivot(Ptr<SpriteComponent> target) const;
	// probInstance.json 의 부모(root) 월드 위치. 스케일 계산에만 사용한다.
	void SetParentWorldPosition(const FVector3D& parentPos);
private:
	Weak<class SpriteComponent> _spriteTarget;
	bool _initialized = false;

	FVector3D	_originPos;
	FVector3D	_originCamPos;

	FVector3D	_parentPos;					// 부모(root) 월드 위치 (스케일 전용)
	bool		_hasParentPos = false;

	FVector3D	_originScale;
	float		_horizonOffset = 0.f;
	float		_horizonStrength = 0.f;
private:
	inline static float _minVisualScale = 0.99f;
	inline static float _maxVisualScale = 2.02f;
private:
	inline static float _parallaxStrength = 0.975f;
	inline static float _maxParallaxOffset = 2048.f;
private:
	inline static float _foregroundYOffset = 1050.f;
	inline static float _parallaxYStrength = 0.4f;
	inline static float _gamePlayPlaneYOffset = 0.f;
private:
	inline static float _foregroundScaleStrength = 0.195f;
	inline static float _backgroundScaleStrength = 0.368f;
private:
	inline static float _foregroundYFollow = 1.f;
	inline static float _backgroundYFollow = 0.5f;
	inline static float _gamePlayPlaneYFollow = 0.f;
private:
	inline static float _gamePlayZ = 0.f;
	inline const static float _gamePlayZDeadZone = PLAYER_DEADZONE;
private:
	inline static float _foregroundParallaxMultiplier = 0.45f;
	inline static float _backgroundParallaxMultiplier = 0.4f;
private:
	inline static float _maxForegroundParallaxAmount = 0.013f;
	inline static float _maxBackgroundParallaxAmount = 0.20f;
};

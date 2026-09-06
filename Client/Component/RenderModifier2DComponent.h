#pragma once
#include "SceneComponent.h"
//FRenderModifier2D			: 렌더 직전에 적용할 보정값 데이터
//RenderModifier2DComponent : Sprite와 Parallax 사이의 공통 인터페이스

struct FRenderModifier2D
{
	bool _enabled = false;
	FVector2D _offset = FVector2D(0.f, 0.f);
	FVector2D _pivot = FVector2D(0.f, 0.f);
	float _scale = 1.f;
};

class RenderModifier2DComponent : public SceneComponent
{
public:
	RenderModifier2DComponent() = default;
	virtual ~RenderModifier2DComponent() = default;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual FRenderModifier2D BuildRenderModifier(Ptr<class SpriteComponent> target)
	{
		return FRenderModifier2D{};
	}
};
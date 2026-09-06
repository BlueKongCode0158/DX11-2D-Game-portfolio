#include "pch.h"
#include "AnimActor.h"
#include "Asset/AssetManager.h"
#include "Asset/Mesh/MeshManager.h"
#include "Asset/Animation/AnimationManager.h"

#include "../Component/Animation/SpriteComponent.h"
#include "../Component/ParallaxLayerComponent.h"

bool AnimActor::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	Actor::Init(id, pos, scale, rot, name);

	_animComp = CreateSceneComponent<SpriteComponent>("AnimComp");
	_animComp->AttachToComponent(_root);
	_animComp->SetUseAnimationFrameTransform(true);

	Ptr<ParallaxLayerComponent> parallaxComp = CreateSceneComponent<ParallaxLayerComponent>("Parallax");
	parallaxComp->AttachToComponent(_root);
	parallaxComp->SetTarget(_animComp);

	return true;
}

void AnimActor::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void AnimActor::Destroy()
{
	Actor::Destroy();
}

void AnimActor::AddAnimation(std::string clipName)
{
	_animComp->AddAnimSequenceAuto(clipName);
}

void AnimActor::SetTint(const FVector4D& tint)
{
	_animComp->SetTint(tint);
}

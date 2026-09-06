#include "pch.h"
#include "ParallaxLayerComponent.h"
#include "Animation/SpriteComponent.h"
#include "../Component/CameraComponent.h"

#include "../World/Level.h"
#include "../Object/Actor.h"
#include "../Object/Asset/Mesh/Mesh.h"

#include "../Common/LogManager.h"
#include "../Editor/EditEngine.h"

ParallaxLayerComponent::ParallaxLayerComponent()
{
}

ParallaxLayerComponent::~ParallaxLayerComponent()
{
}

FRenderModifier2D ParallaxLayerComponent::BuildRenderModifier(Ptr<class SpriteComponent> target)
{
	FRenderModifier2D result;

	if (!target)
	{
		return result;
	}

	Ptr<IComponentOwner> owner = GetOwner();
	if (!owner)
	{
		return result;
	}

	Ptr<Level> level = owner->GetLevel();
	if (!level || !level->GetMainCamera())
	{
		return result;
	}

	Ptr<Actor> ownerActor = Cast<IComponentOwner, Actor>(owner);
	if (!ownerActor)
	{
		return result;
	}

	const FVector3D camPos = level->GetCameraWorldPos();
	if (!_initialized)
	{
		_originCamPos = camPos;
		_originPos = ownerActor->GetWorldPosition();
		_initialized = true;
	}

	const float refDist = max(fabsf(_gamePlayZ - _originCamPos._z), 1.f);
	const  float zFromGamePlay = _originPos._z - _gamePlayZ;

	const bool isGamePlayPlane = fabsf(zFromGamePlay) <= _gamePlayZDeadZone;

	const float foregroundDepth = Utility::Clamp(-zFromGamePlay / refDist, 0.f, 1.f);
	const float backgroundDepth = Utility::Clamp(zFromGamePlay / refDist, 0.f, 1.f);

	const FVector2D camDelta = { camPos._x - _originCamPos._x,
							 camPos._y - _originCamPos._y };

	float parallaxAmount = 0.f;

	if (!isGamePlayPlane)
	{
		if (foregroundDepth > 0.f)
		{
			parallaxAmount = -foregroundDepth * _parallaxStrength * _foregroundParallaxMultiplier;
			parallaxAmount = Utility::Clamp(parallaxAmount, -_maxForegroundParallaxAmount, 0.f);
		}
		else if (backgroundDepth > 0.f)
		{
			parallaxAmount = backgroundDepth * _parallaxStrength * _backgroundParallaxMultiplier;
			parallaxAmount = Utility::Clamp(parallaxAmount, 0.f, _maxBackgroundParallaxAmount);
		}
	}

	float yParallaxAmount = 0.f;

	if (foregroundDepth > 0.f)
	{
		yParallaxAmount = parallaxAmount * _parallaxYStrength;
		yParallaxAmount += (-parallaxAmount) * _foregroundYFollow;
		yParallaxAmount = Utility::Clamp(yParallaxAmount, -0.015f, 0.07f);
	}
	else if (backgroundDepth > 0.f)
	{
		yParallaxAmount = parallaxAmount * _parallaxYStrength * _backgroundYFollow;
	}
	else
	{
		yParallaxAmount = _gamePlayPlaneYFollow;
	}

	FVector2D offset = {
		Utility::Clamp(camDelta._x * parallaxAmount, -_maxParallaxOffset, _maxParallaxOffset),
		Utility::Clamp(camDelta._y * yParallaxAmount, -_maxParallaxOffset, _maxParallaxOffset)};

	offset._y -= foregroundDepth * _foregroundYOffset;

	if (isGamePlayPlane)
	{
		offset._y -= _gamePlayPlaneYOffset;
	}

	// 스케일은 부모(root) 위치값이 있으면 그 깊이로 계산한다 (그룹 단위로 스케일 일관성 유지).
	// offset / parallax 이동은 그대로 자기 위치(_originPos) 기반을 사용한다.
	const float scaleZFromGamePlay = /*(_hasParentPos ? _parentPos._z : */_originPos._z /*)*/ - _gamePlayZ;
	const float scaleForegroundDepth = Utility::Clamp(-scaleZFromGamePlay / refDist, 0.f, 1.f);
	const float scaleBackgroundDepth = Utility::Clamp(scaleZFromGamePlay / refDist, 0.f, 1.f);

	const float visualScale = 1.f + scaleForegroundDepth * _foregroundScaleStrength - scaleBackgroundDepth * _backgroundScaleStrength;

	result._enabled = true;
	result._offset = offset;
	result._pivot = (_hasParentPos && false == _parentPos.IsZero()) ? FVector2D(_parentPos._x, _parentPos._y) : BuildBottomPivot(target);
	result._scale = Utility::Clamp(visualScale, _minVisualScale, _maxVisualScale);

	return result;
}

void ParallaxLayerComponent::DrawGlobalInspector()
{
	ImGui::SeparatorText("Parallax Global");

	ImGui::DragFloat("Parallax Strength", &_parallaxStrength, 0.001f, 0.f, 2.f);
	ImGui::DragFloat("Max Offset", &_maxParallaxOffset, 1.f, 0.f, 4096.f);

	ImGui::SeparatorText("X Movement");
	ImGui::DragFloat("Foreground X Mult", &_foregroundParallaxMultiplier, 0.001f, 0.f, 2.f);
	ImGui::DragFloat("Background X Mult", &_backgroundParallaxMultiplier, 0.001f, 0.f, 2.f);
	ImGui::DragFloat("Max Foreground X", &_maxForegroundParallaxAmount, 0.001f, -1.f, 0.2f);
	ImGui::DragFloat("Max Background X", &_maxBackgroundParallaxAmount, 0.001f, 0.f, 0.2f);

	ImGui::SeparatorText("Y Movement");
	ImGui::DragFloat("Y Strength", &_parallaxYStrength, 0.001f, 0.f, 3.f);
	ImGui::DragFloat("Foreground Y Follow", &_foregroundYFollow, 0.001f, 0.f, 1.f);
	ImGui::DragFloat("Background Y Follow", &_backgroundYFollow, 0.001f, -1.f, 1.f);
	ImGui::DragFloat("Foreground Y Offset", &_foregroundYOffset, 0.5f, 0.f, 2000.f);
	ImGui::DragFloat("Gameplay Y Offset", &_gamePlayPlaneYOffset, 0.5f, -100.f, 100.f);

	ImGui::SeparatorText("Scale");
	ImGui::DragFloat("Foreground Scale", &_foregroundScaleStrength, 0.001f, 0.f, 1.f);
	ImGui::DragFloat("Background Scale", &_backgroundScaleStrength, 0.001f, 0.f, 1.f);
	ImGui::DragFloat("Min Visual Scale", &_minVisualScale, 0.001f, 0.01f, 2.f);
	ImGui::DragFloat("Max Visual Scale", &_maxVisualScale, 0.001f, 0.01f, 3.f);

	ImGui::SeparatorText("Depth");
	ImGui::DragFloat("Gameplay Z", &_gamePlayZ, 0.1f, -100.f, 100.f);
	//ImGui::DragFloat("Gameplay DeadZone", &_gamePlayZDeadZone, 0.01f, 0.f, 10.f);
}

bool ParallaxLayerComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	_type = eComponentType::PARALLAX;
	return true;
}

void ParallaxLayerComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
}

bool ParallaxLayerComponent::Save(std::ofstream& file)
{
	return true;
}

bool ParallaxLayerComponent::Load(std::ifstream& file)
{
	return true;
}

void ParallaxLayerComponent::SetTarget(Ptr<SpriteComponent> target)
{
	if (!target)
	{
		return;
	}

	_spriteTarget = target;
	target->SetRenderModifier(This<RenderModifier2DComponent>());
	SetEnable(false);
}

Ptr<SpriteComponent> ParallaxLayerComponent::GetTarget()
{
	return Lock<SpriteComponent>(_spriteTarget);
}

void ParallaxLayerComponent::SetParentWorldPosition(const FVector3D& parentPos)
{
	_parentPos = parentPos;
	_hasParentPos = true;
}

FVector2D ParallaxLayerComponent::BuildBottomPivot(Ptr<SpriteComponent> target) const
{
	if (!target || !target->GetMesh())
	{
		return FVector2D(_originPos._x, _originPos._y);
	}

	const FLocalBound2D& local = target->GetMesh()->GetLocalBound();

	FVector3D bottomCenter = {
		(local._Min._x + local._Max._x) * 0.5f,
		local._Min._y, 0.f
	};

	bottomCenter = bottomCenter.TransformCoord(target->GetWorldMatrix());
	return FVector2D(bottomCenter._x, bottomCenter._y);
}

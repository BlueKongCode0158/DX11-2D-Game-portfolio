#include "pch.h"
#include "SpriteComponent.h"

#include "../../Object/Asset/Mesh/Mesh.h"
#include "../../Object/Asset/AssetManager.h"
#include "../../Object/Asset/Texture/Texture.h"

#include "../../Shader/ShaderManager.h"
#include "../../Shader/ConstantBuffer/SpriteCBuffer.h"
#include "../../Shader/ConstantBuffer/TransformCBuffer.h"

#include "../../World/Level.h"
#include "../../Editor/EditEngine.h"
#include "../RenderModifier2DComponent.h"


SpriteComponent::SpriteComponent()
{
	_isRender = true;
}

SpriteComponent::~SpriteComponent()
{
}

bool SpriteComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	_spriteCBuffer = ShaderManager::Instance().FindCBuffer<SpriteCBuffer>("Sprite");
	SetShader("SpriteShader");
	_mesh = MESH_MANAGER->FindMesh("TexRect");
	_type = eComponentType::SPRITE;
	return true;
}

void SpriteComponent::Tick(float deltaTime)
{
	if (_animation)
	{
		_animation->Tick(deltaTime);
		if (_useAnimationFrameTransform)
		{
			ApplyAnimationFrameTransform();
		}
	}
	SceneComponent::Tick(deltaTime);
}

void SpriteComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);
	
	if (_animation)
	{
		_animation->SetShader();
	}


	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
	_spriteCBuffer->SetTint(_tint);

	FRenderModifier2D modifier = BuildRenderModifier();
	if (modifier._enabled)
	{
		_spriteCBuffer->SetParallaxOffset(modifier._offset._x, modifier._offset._y);
		_spriteCBuffer->SetParallaxPivot(modifier._pivot._x, modifier._pivot._y);
		_spriteCBuffer->SetParallaxScale(modifier._scale);
	}
	else
	{
		_spriteCBuffer->SetParallaxOffset(0.f, 0.f);
		_spriteCBuffer->SetParallaxPivot(0.f, 0.f);
		_spriteCBuffer->SetParallaxScale(1.f);
	}
	_spriteCBuffer->Update();
	
	_transformCBuffer->SetWorldMatrix(_matrix._world);
	Ptr<Level> level = Lock<Level>(_level);

	if (nullptr == level)
	{
		return;
	}

	FMatrix view = level->GetViewMatrix();
	FMatrix proj = level->GetProjMatrix();

	_transformCBuffer->SetViewMatrix(view);
	_transformCBuffer->SetProjMatrix(proj);
	_transformCBuffer->Update();

	_shader->SetShader();

	if (_texture)
	{
		_texture->SetShader(0, SHADER_TYPE::PIXEL, _textureIndex);
	}
	_mesh->Render();
}

void SpriteComponent::DrawInspector()
{
	SceneComponent::DrawInspector();
	ImGui::SeparatorText("SpriteComponent");

	ImGui::SeparatorText("Tint");
	if (ImGui::ColorEdit4("Tint", &_tint._x))
	{
		SetTint(_tint);
	}

	if (_animation)
	{
		_animation->DrawInspector();

		ImGui::DragFloat3("Animation Pivot", &_animationPivot._x, 0.1f, 1000.f);
	}
	
}

void SpriteComponent::Destroy()
{
	SceneComponent::Destroy();
	DESTROY(_animation);
}

bool SpriteComponent::Save(std::ofstream& file)
{
	return true;
}

bool SpriteComponent::Load(std::ifstream& file)
{
	return true;
}

bool SpriteComponent::BuildRenderBound2D(OUT FLocalBound2D& out) const
{
	if (!_mesh)
	{
		return false;
	}

	const FLocalBound2D& local = _mesh->GetLocalBound();

	FVector3D corners[4] = {
		{local._Min._x, local._Min._y, 0.f},
		{local._Min._x, local._Max._y, 0.f},
		{local._Max._x, local._Min._y, 0.f},
		{local._Max._x, local._Max._y, 0.f}
	};

	out.Reset();
	FMatrix world = GetWorldMatrix();
	for (const FVector3D& corner : corners)
	{
		FVector3D result = corner.TransformCoord(world);
		out.Expand(result._x, result._y);
	}

	return out.IsValid();
}

void SpriteComponent::SetRenderModifier(Ptr<class RenderModifier2DComponent> modifier)
{
	_renderModifier = modifier;
}

FRenderModifier2D SpriteComponent::BuildRenderModifier()
{
	Ptr<RenderModifier2DComponent> modifier = Lock< RenderModifier2DComponent>(_renderModifier);

	if (!modifier)
	{
		return FRenderModifier2D{};
	}

	return modifier->BuildRenderModifier(This<SpriteComponent>());
}

void SpriteComponent::SetMesh(const std::string& AtlasName, int32 pathID)
{
	auto mesh = MESH_MANAGER->FindMesh(AtlasName, pathID);
	if (nullptr == mesh)
	{
		return;
	}
	_mesh = mesh;
	MarkRenderBoundDirty();
}

void SpriteComponent::SetShader(const std::string& name)
{
	_shaderName = name;
	_shader = ShaderManager::Instance().FindShader(name);
}

const std::string& SpriteComponent::GetShaderName() const
{
	return _shaderName;
}

void SpriteComponent::SetTint(const FVector4D& tint)
{
	SetTint(tint._x, tint._y, tint._z, tint._w);
}

void SpriteComponent::SetTint(float x, float y, float z, float w)
{
	_tint._x = x;
	_tint._y = y;
	_tint._z = z;
	_tint._w = w;
}

void SpriteComponent::SetOpacity(float op)
{
	_tint._w = op;
}

void SpriteComponent::SetTexture(const std::string& name, int textureIndex)
{
	_texture = TEXTURE_MANAGER->FindTexture(name);
	_textureIndex = textureIndex;
}

void SpriteComponent::SetTexture(Ptr<class Texture> texture, int textureIndex)
{
	_texture = texture;
	_textureIndex = textureIndex;
}

void SpriteComponent::SetTextureIndex(int32 index)
{
	_textureIndex = index;
}

void SpriteComponent::AddAnimSequence(const std::string& name, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->AddSequence(name, loop, reverse, playTime, playRate, loopFrame);
}

void SpriteComponent::AddAnimSequence(const std::string& sequenceName, const std::string& dataName, int32 startFrame, int32 frameCount, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->AddSequence(sequenceName, dataName, startFrame, frameCount, loop, reverse, playTime, playRate, loopFrame);
}

void SpriteComponent::AddAnimSequence(Ptr<class Animation2DData>& data, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->AddSequence(data, loop, reverse, playTime, playRate, loopFrame);
}

void SpriteComponent::AddAnimSequenceAuto(const std::string& name, bool reverse, float playRate)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->AddSequenceAuto(name, reverse, playRate);
}

void SpriteComponent::SetLoopAnimationFrame(const std::string& name, int32 loopFrame)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->SetLoopAnimationFrame(name, loopFrame);
}

void SpriteComponent::SetPlayTime(const std::string& name, float time)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->SetPlayTime(name, time);
}

void SpriteComponent::SetPlayRate(const std::string& name, float rate)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->SetPlayRate(name, rate);
}

void SpriteComponent::SetLoop(const std::string& name, bool loop)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->SetLoop(name, loop);
}

void SpriteComponent::SetReverse(const std::string& name, bool reverse)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->SetReverse(name, reverse);
}

void SpriteComponent::ChangeAnimation(const std::string& name)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}

	_animation->ChangeAnimation(name);
	ApplyAnimationFrameTransform();
}

void SpriteComponent::SetPlay(const std::string& name, bool play)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}
	_animation->SetPlay(name,play);
}

void SpriteComponent::SetAnimFlip(bool flip)
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}
	_animation->SetAnimFlip(flip);
}

void SpriteComponent::SetAnimPivot(FVector3D pivot)
{
	SetAnimPivot(pivot._x, pivot._y, pivot._z);
}

void SpriteComponent::SetAnimPivot(FVector2D pivot)
{	
	SetAnimPivot(pivot._x, pivot._y, 0.f);
	
}

void SpriteComponent::SetAnimPivot(float x, float y, float z)
{
	_animationPivot._x = x;
	_animationPivot._y = y;
	_animationPivot._z = z;
}

bool SpriteComponent::GetAnimFlip()
{
	if (!_animation)
	{
		_animation = CreateAnimation();
	}
	return _animation->GetAnimFlip();
}

void SpriteComponent::AddFrameSize(float deltaTime)
{
	_animation->AddFrameSize(deltaTime);
}

Ptr<class Animation2D> SpriteComponent::CreateAnimation()
{
	_animation = New<Animation2D>();
	_animation->_owner = This<SpriteComponent>();
	if (!_animation->Init())
	{
		DESTROY(_animation);
		return nullptr;
	}
	return _animation;
}

Ptr<class Animation2D> SpriteComponent::GetAnimation()
{
	return _animation;
}

Ptr<class Mesh> SpriteComponent::GetMesh()
{
	return _mesh;
}

void SpriteComponent::SetMesh(const std::string& name)
{
	Ptr<Mesh> mesh = MESH_MANAGER->FindMesh(name);
	if (mesh)
	{
		_mesh = mesh;
	}
	MarkRenderBoundDirty();
	return;
}

void SpriteComponent::ApplyAnimationFrameTransform()
{
	if (!_animation)
	{
		return;
	}

	Ptr<Animation2DData> data = _animation->GetCurrentSequneceData();
	if (!data)
	{
		return;
	}

	const int32 curFrame = _animation->GetCurrentSequneceDataFrame();
	const FAnimationFrame& frame = data->GetFrame(curFrame);

	if (frame._draw._x != 0.f && frame._draw._y != 0.f)
	{
		SetRelativeScale(frame._draw._x, frame._draw._y);
	}

	const bool isFlip = _animation->GetAnimFlip();
	const float offsetX = isFlip ? -frame._offset._x : frame._offset._x;

	SetRelativePosition(_animationPivot._x + offsetX, _animationPivot._y + frame._offset._y, GetRelativePosition()._z);

}

void SpriteComponent::SetUseAnimationFrameTransform(bool enable)
{
	_useAnimationFrameTransform = enable;
}

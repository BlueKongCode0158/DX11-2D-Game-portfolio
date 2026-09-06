#include "pch.h"
#include "ParticleComponent.h"
#include "../Core/Device.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/ConstantBuffer/ParticleCBuffer.h"
#include "../Shader/StructBuffer/ParticleSBuffer.h"
#include "../Render/RenderManager.h"
#include "../Render/BlendState.h"
#include "../Object/Asset/Texture/Texture.h"
#include "../Object/Asset/Texture/TextureManager.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Object/Asset/AssetManager.h"
#include "../Editor/EditEngine.h"		// ImGui
#include "../Object/Asset/Animation/Animation2DData.h"

#include "../Core/GameEngine.h"
#include "../World/World.h"
#include "../World/Level.h"

#include <algorithm>

static float RandFloat()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

ParticleComponent::ParticleComponent()
{
	_type = eComponentType::PARTICLE;
	_isRender = true;
}

ParticleComponent::~ParticleComponent()
{
}

bool ParticleComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	if (!SceneComponent::Init(id, name, owner))
	{
		return false;
	}

	_mesh = MESH_MANAGER->FindMesh("TexRect");
	_shader = FIND_SHADER("ParticleShader");
	_transformCBuffer = FIND_CBUFFER("Transform", TransformCBuffer);
	_particleCBuffer = FIND_CBUFFER("Particle", ParticleCBuffer);
	_particleSBuffer = FIND_SBUFFER("Particle", ParticleSBuffer);

	_particles.resize(_emitProperty._maxCount);
	return true;
}

void ParticleComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);

	if (!_isPlaying)
	{
		return;
	}

	for (auto& particle : _particles)
	{
		if (!particle._isAlive)
		{
			continue;
		}

		if (_emitProperty._animData)
		{
			int frameCount = _emitProperty._animData->GetFrameCount();
			if (_emitProperty._animSyncLifetime)
			{
				float ratio = 1.f - (particle._lifeTime / particle._maxLifeTime);
				particle._animFrame = min(static_cast<int>(ratio * frameCount), frameCount - 1);
			}
			else
			{
				float fps = (_emitProperty._animFPS > 0.f) ? _emitProperty._animFPS : _emitProperty._animData->GetFps();

				particle._animTime += deltaTime;
				if (fps > 0.f)
				{
					float frameTime = 1.f / fps;
					while (particle._animTime >= frameTime)
					{
						particle._animTime -= frameTime;
						particle._animFrame++;

						if (particle._animFrame >= frameCount)
						{
							particle._animFrame = _emitProperty._animLoop ? 0 : frameCount - 1;
						}
					}
				}
			}
		}

		particle._lifeTime -= deltaTime;
		if (particle._lifeTime <= 0)
		{
			particle._isAlive = false;
			continue;
		}

		float ratio = 1.f - (particle._lifeTime / particle._maxLifeTime);

		particle._position._x += particle._velocity._x * deltaTime;
		particle._position._y += particle._velocity._y * deltaTime;
		particle._position._y += _emitProperty._gravity * deltaTime;

		particle._color = EvaluateColorKeys(ratio);
		particle._size = particle._sizeStart + (_emitProperty._sizeEnd - particle._sizeStart) * ratio;

	}

	bool canSpawn = _emitProperty._isLoop || _elapsedTime < _emitProperty._duration;

	if (canSpawn)
	{
		// 초당 spawnRate 개를 deltaTime 단위로 누적한다. 누적값의 정수부만큼
		// 이번 프레임에 스폰하고, 소수부는 다음 프레임으로 이월한다.
		_spawnAccum += _emitProperty._spawnRate * deltaTime;
		int32 count = static_cast<int32>(_spawnAccum);
		_spawnAccum -= static_cast<float>(count);

		for (int32 i = 0; i < count; i++)
		{
			SpawnOne();
		}

		_elapsedTime += deltaTime;
	}
}

void ParticleComponent::Render(float deltaTime)
{
	_particleSBuffer->Clear();

	for (const auto& particle : _particles)
	{
		if (!particle._isAlive)
		{
			continue;
		}

		FVector2D UVLT = { 0.f, 0.f };
		FVector2D UVRB = { 1.f, 1.f };

		if (_emitProperty._animData && _emitProperty._animData->GetFrameCount() > 0)
		{
			int particleFrame = Utility::Clamp<int>(particle._animFrame, 0, _emitProperty._animData->GetFrameCount() - 1);
			const FAnimationFrame& animationFrame = _emitProperty._animData->GetFrame(particleFrame);

			Ptr<Texture> tex = _emitProperty._animData->GetTexture();
			if (tex)
			{
				float width = static_cast<float>(tex->GetWidth());
				float height = static_cast<float>(tex->GetHeight());

				UVLT._x = animationFrame._start._x / width;
				UVLT._y = animationFrame._start._y / height;
				UVRB._x = UVLT._x + animationFrame._size._x / width;
				UVRB._y = UVLT._y + animationFrame._size._y / height;
			}
		}

		_particleSBuffer->AddInstance({ particle._position, particle._size, particle._color, particle._rotation, UVLT, UVRB});
	}

	_aliveCount = _particleSBuffer->GetCount();
	if (_aliveCount == 0)
	{
		return;
	}

	_particleSBuffer->Update();

	if (_emitProperty._moveWithTransform)
	{
		_transformCBuffer->SetWorldMatrix(GetWorldMatrix());
	}
	else
	{
		FMatrix identity;
		_transformCBuffer->SetWorldMatrix(identity);
	}

	// 오버라이드(에디터 프리뷰)가 있으면 그 카메라를, 없으면 현재 레벨 카메라를 쓴다.
	if (_useCameraOverride)
	{
		_transformCBuffer->SetViewMatrix(_overrideView);
		_transformCBuffer->SetProjMatrix(_overrideProj);
	}
	else if (Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel())
	{
		_transformCBuffer->SetViewMatrix(level->GetViewMatrix());
		_transformCBuffer->SetProjMatrix(level->GetProjMatrix());
	}
	_transformCBuffer->Update();

	bool useTexture = _texture != nullptr;
	_particleCBuffer->SetUseTexture(useTexture);
	_particleCBuffer->Update();

	_shader->SetShader();
	_particleSBuffer->Bind();

	Ptr<BlendState> blend = RenderManager::Instance().GetAlphaBlend(_eBlendState);
	if (blend)
	{
		blend->SetState();
	}

	if (useTexture)
	{
		// 셰이더의 gBaseSample(s0) 에 샘플러를 묶어줘야 Sample() 결과가 나온다.
		// (안 묶으면 검정/투명이 곱해져 텍스처 파티클이 안 보인다.)
		ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
		_texture->SetShader(0, SHADER_TYPE::PIXEL, 0);
	}

	_mesh->RenderInstanced(_aliveCount);
	
	if (blend)
	{
		blend->ResetState();
	}
}

void ParticleComponent::DrawInspector()
{
	SceneComponent::DrawInspector();

	ImGui::SeparatorText("Particle");

	// 재생 제어
	if (_isPlaying)
	{
		if (ImGui::Button("Stop"))
		{
			Stop();
		}
	}
	else
	{
		if (ImGui::Button("Play"))
		{
			Play();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Restart"))
	{
		Play();
	}
	ImGui::Text("Alive : %d / %d", static_cast<int32>(_aliveCount), _emitProperty._maxCount);

	// 텍스처 선택 : TextureManager 에 로드된 텍스처를 골라 얹는다. (None 이면 색상만)
	ImGui::SeparatorText("Texture");
	Ptr<TextureManager> texMgr = TEXTURE_MANAGER;
	if (texMgr)
	{
		const auto& textures = texMgr->GetTextures();

		std::string curName = "None";
		for (const auto& it : textures)
		{
			if (it.second == _texture)
			{
				curName = it.first;
				break;
			}
		}

		if (ImGui::BeginCombo("Texture", curName.c_str()))
		{
			if (ImGui::Selectable("None", _texture == nullptr))
			{
				SetTexture(Ptr<Texture>());	// 텍스처 해제
			}

			std::vector<std::string> names;
			names.reserve(textures.size());
			for (const auto& it : textures)
			{
				names.push_back(it.first);
			}
			std::sort(names.begin(), names.end());

			for (const std::string& name : names)
			{
				const bool selected = (name == curName);
				if (ImGui::Selectable(name.c_str(), selected))
				{
					SetTexture(name);
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	// 속성 편집. SetEmitProperty 는 파티클을 clear/resize 하며 타이머를 리셋하므로
	// (매 프레임 호출하면 색만 바꿔도 파티클이 끊긴다) 여기서는 _emitProperty 를
	// 직접 편집하고, 버퍼 크기에 영향을 주는 _maxCount 가 바뀐 경우에만 resize 한다.
	int32 prevMax = _emitProperty._maxCount;
	if (DrawParticleEmitPropertyUI(_emitProperty))
	{
		SetBlendMode(_emitProperty._blendMode);
		if (_emitProperty._maxCount != prevMax)
		{
			if (_emitProperty._maxCount < 1)
			{
				_emitProperty._maxCount = 1;
			}
			_particles.resize(_emitProperty._maxCount);
		}
	}
}

void ParticleComponent::Destroy()
{
}

bool ParticleComponent::BuildRenderBound2D(OUT FLocalBound2D& out) const
{
	return false;
}

void ParticleComponent::SetEmitProperty(const FParticleEmitProperty& prop)
{
	_emitProperty = prop;
	SetBlendMode(prop._blendMode);

	_particles.clear();
	_particles.resize(prop._maxCount);
	
	_spawnAccum = 0.f;
	_elapsedTime = 0.f;
}

void ParticleComponent::SetTexture(const std::string& name)
{
	_texture = TEXTURE_MANAGER->FindTexture(name);
}

void ParticleComponent::SetTexture(Ptr<class Texture> texture)
{
	_texture = texture;
}

void ParticleComponent::SetAnimation(Ptr<class Animation2DData> animData)
{
	_emitProperty._animData = animData;
}

void ParticleComponent::Play()
{
	_isPlaying = true;
	_elapsedTime = 0.f;
	_spawnAccum = 0.f;
}

void ParticleComponent::Stop()
{
	_isPlaying = false;
}

bool ParticleComponent::IsPlaying() const
{
	return _isPlaying;
}

void ParticleComponent::SpawnOne()
{
	for (auto& particle : _particles)
	{
		if (particle._isAlive)
		{
			continue;
		}

		particle._animTime = 0.f;
		particle._animFrame = 0;
		particle._isAlive = true;
		particle._maxLifeTime = _emitProperty._lifeTime.Evaluate();
		particle._lifeTime = particle._maxLifeTime;

		float spawnX = FParticleMinMax{ _emitProperty._spawnAreaMin._x, _emitProperty._spawnAreaMax._x }.Evaluate();
		float spawnY = FParticleMinMax{ _emitProperty._spawnAreaMin._y, _emitProperty._spawnAreaMax._y }.Evaluate();

		if (_emitProperty._moveWithTransform)
		{
			particle._position = { spawnX, spawnY, 0.f };
		}
		else
		{
			FVector3D worldPos = GetWorldPosition();
			particle._position = {
				worldPos._x + spawnX,
				worldPos._y + spawnY,
				worldPos._z
			};
		}

		// 기준 방향(direction)을 각도로 환산한 뒤, ±spread(라디안) 범위로 분사한다.
		float baseAngle = atan2f(_emitProperty._direction._y,
			_emitProperty._direction._x);
		float angle = baseAngle + _emitProperty._spread * (2.f * RandFloat() - 1.f);
		float speed = _emitProperty._speed.Evaluate();
		particle._velocity = { cosf(angle) * speed, sinf(angle) * speed };

		// 크기 - 회전 - 색상
		particle._sizeStart = _emitProperty._size.Evaluate();
		particle._size = particle._sizeStart;
		particle._rotation = _emitProperty._rotation.Evaluate();
		particle._color = EvaluateColorKeys(0.f);

		return;
	}
}


FVector4D ParticleComponent::EvaluateColorKeys(float t) const
{
	const auto& keys = _emitProperty._colorKeys;
	if (keys.empty())
	{
		return { 1.f, 1.f, 1.f, 1.f };
	}

	if (t <= keys.front()._time)
	{
		return keys.front()._color;
	}
	if (t >= keys.back()._time)
	{
		return keys.back()._color;
	}

	size_t size = static_cast<size_t>(keys.size());
	for (int32 i = 0; i < size - 1; i++)
	{
		if (t >= keys[i]._time && t <= keys[i + 1]._time)
		{
			float localT = (t - keys[i]._time) / (keys[i + 1]._time - keys[i]._time);
			const FVector4D& curColor = keys[i]._color;
			const FVector4D& nextColor = keys[i + 1]._color;
			return
			{
				curColor._x + (nextColor._x - curColor._x) * localT,
				curColor._y + (nextColor._y - curColor._y) * localT,
				curColor._z + (nextColor._z - curColor._z) * localT,
				curColor._w + (nextColor._w - curColor._w) * localT
			};
		}
	}
	return keys.back()._color;
}

bool DrawParticleEmitPropertyUI(FParticleEmitProperty& prop)
{
	bool changed = false;

	ImGui::SeparatorText("Emit");
	changed |= ImGui::InputInt("MaxCount", &prop._maxCount);
	changed |= ImGui::DragFloat("SpawnRate", &prop._spawnRate, 0.5f, 0.f, 1000.f);
	changed |= ImGui::Checkbox("Loop", &prop._isLoop);
	if (!prop._isLoop)
	{
		ImGui::SameLine();
		changed |= ImGui::DragFloat("Duration", &prop._duration, 0.05f, 0.f, 100.f);
	}
	changed |= ImGui::Checkbox("MoveWithTransform", &prop._moveWithTransform);

	ImGui::SeparatorText("Life / Speed / Size / Rotation");

	// FParticleMinMax 는 { _min, _max } 연속 float 2개라 DragFloat2 로 한 줄에 편집한다.
	changed |= ImGui::DragFloat2("LifeTime", &prop._lifeTime._min, 0.05f, 0.f, 100.f);
	changed |= ImGui::DragFloat2("Speed", &prop._speed._min, 0.05f, 0.f, 100.f);
	changed |= ImGui::DragFloat2("Size", &prop._size._min, 0.01f, 0.f, 100.f);
	changed |= ImGui::DragFloat("SizeEnd", &prop._sizeEnd, 0.01f, 0.f, 100.f);
	changed |= ImGui::DragFloat2("Rotation", &prop._rotation._min, 0.01f, -6.283f, 6.283f);

	ImGui::SeparatorText("Shape");
	changed |= ImGui::DragFloat2("SpawnAreaMin", &prop._spawnAreaMin._x, 0.05f);
	changed |= ImGui::DragFloat2("SpawnAreaMax", &prop._spawnAreaMax._x, 0.05f);
	changed |= ImGui::DragFloat2("Direction", &prop._direction._x, 0.05f, -1.f, 1.f);
	changed |= ImGui::DragFloat("Spread", &prop._spread, 0.01f, 0.f, 6.283f);
	changed |= ImGui::DragFloat("Gravity", &prop._gravity, 0.001f, -10.f, 10.f);

	ImGui::SeparatorText("Blend");
	const char* blendItems[] = { "DEFAULT", "LIGHT" };
	int32 blend = static_cast<int32>(prop._blendMode);
	if (blend < 0 || blend > 1)
	{
		blend = 0;
	}
	if (ImGui::Combo("BlendMode", &blend, blendItems, IM_ARRAYSIZE(blendItems)))
	{
		prop._blendMode = static_cast<eBlendMode>(blend);
		changed = true;
	}

	ImGui::SeparatorText("Color Keys");
	int32 removeIndex = -1;
	for (int32 i = 0; i < static_cast<int32>(prop._colorKeys.size()); ++i)
	{
		ImGui::PushID(i);
		FColorKey& key = prop._colorKeys[i];
		changed |= ImGui::DragFloat("Time", &key._time, 0.01f, 0.f, 1.f);
		changed |= ImGui::ColorEdit4("Color", &key._color._x);
		ImGui::SameLine();
		if (ImGui::Button("X"))
		{
			removeIndex = i;
		}
		ImGui::PopID();
	}

	if (removeIndex >= 0)
	{
		prop._colorKeys.erase(prop._colorKeys.begin() + removeIndex);
		changed = true;
	}

	if (ImGui::Button("Add Color Key"))
	{
		float time = prop._colorKeys.empty() ? 0.f : prop._colorKeys.back()._time;
		prop._colorKeys.push_back({ time, { 1.f, 1.f, 1.f, 1.f } });
		changed = true;
	}

	return changed;
}

FParticleEmitProperty MakeSmokePreset()
{
	FParticleEmitProperty p;
	p._maxCount = 20;
	p._spawnRate = 5.f;
	p._lifeTime = { 2.f, 4.f };
	p._speed = { 0.f, 0.f };
	p._size = { 0.4f, 0.7f };
	p._sizeEnd = 0.8f;
	p._rotation = { 0.f, 6.283f };
	p._gravity = 0.f;
	p._blendMode = eBlendMode::DEFAULT;
	p._isLoop = true;
	p._moveWithTransform = true;
	p._colorKeys = {
		{ 0.00f, { 0.5f, 0.3f, 0.1f, 0.00f } },
		{ 0.31f, { 0.5f, 0.3f, 0.1f, 0.87f } },
		{ 0.70f, { 0.5f, 0.3f, 0.1f, 0.81f } },
		{ 1.00f, { 0.5f, 0.3f, 0.1f, 0.00f } },
	};
	return p;
}

// Hollow Knight 먼지 점
FParticleEmitProperty MakeDustPreset()
{
	FParticleEmitProperty p;
	p._maxCount = 14;
	p._spawnRate = 40.f;
	p._lifeTime = { 3.f, 6.f };
	p._speed = { 0.01f, 0.3f };
	p._size = { 0.4f, 0.7f };
	p._sizeEnd = 0.f;
	p._gravity = -0.001f;
	p._spread = 0.436f;
	p._blendMode = eBlendMode::DEFAULT;
	p._isLoop = true;
	p._moveWithTransform = true;
	p._colorKeys = {
		{ 0.00f, { 0.93f, 0.58f, 0.27f, 0.00f } },
		{ 0.31f, { 0.93f, 0.58f, 0.27f, 0.87f } },
		{ 0.70f, { 0.93f, 0.58f, 0.27f, 0.81f } },
		{ 1.00f, { 0.93f, 0.58f, 0.27f, 0.00f } },
	};
	return p;
}

// 불꽃 (Additive)
FParticleEmitProperty MakeFirePreset()
{
	FParticleEmitProperty p;
	p._maxCount = 50;
	p._spawnRate = 30.f;
	p._lifeTime = { 0.3f, 0.8f };
	p._speed = { 1.f, 3.f };
	p._size = { 0.1f, 0.3f };
	p._sizeEnd = 0.f;
	p._direction = { 0.f, 1.f };
	p._spread = 0.5f;
	p._gravity = 0.5f;
	p._blendMode = eBlendMode::LIGHT;   // Additive
	p._isLoop = true;
	p._moveWithTransform = true;
	p._colorKeys = {
		{ 0.0f, { 1.0f, 0.8f, 0.2f, 1.0f } },
		{ 0.5f, { 1.0f, 0.3f, 0.0f, 0.8f } },
		{ 1.0f, { 0.2f, 0.0f, 0.0f, 0.0f } },
	};
	return p;
}

FParticleEmitProperty MakeWhitePalacePreset()
{
	FParticleEmitProperty p;
	p._maxCount = 105;
	p._spread = 20.f;
	p._lifeTime = { 5.f, 3.f };
	p._speed = { 0.1f, 0.f };
	p._size = { 0.3f, 0.22f };
	p._sizeEnd = 0.f;
	p._direction = { 1.f, 1.f };
	p._spread = 1.f;
	p._gravity = 0.f;
	p._blendMode = eBlendMode::DEFAULT;
	p._isLoop = true;
	p._spawnAreaMin = { -5.35f, -5.35f };
	p._spawnAreaMax = { 5.35f, 5.35f };
	p._colorKeys = { {0.f, {0.6764706f,0.8125759f, 1.f, 0.8078431f}} };
	p._moveWithTransform = true;
	return p;
}

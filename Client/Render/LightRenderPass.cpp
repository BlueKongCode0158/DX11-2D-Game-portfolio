#include "pch.h"
#include "LightRenderPass.h"
#include "RenderTarget2D.h"
#include "../Component/Light2DComponent.h"
#include "../Component/SceneComponent.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/ConstantBuffer/SpriteCBuffer.h"
#include "../Shader/ShaderManager.h"
#include "../Render/BlendState.h"

#include "../Object/Asset/Mesh/Mesh.h"
#include "../Object/Asset/Texture/Texture.h"

#include "../World/World.h"
#include "../World/Level.h"

#include "../Core/Device.h"
#include "../Core/GameEngine.h"
#include "RenderManager.h"


LightRenderPass::LightRenderPass()
{
}

LightRenderPass::~LightRenderPass()
{
}

bool LightRenderPass::Init()
{
	_lightTarget = New<RenderTarget2D>();
	if (!_lightTarget->Init())
	{
		return false;
	}

	_shader = FIND_SHADER("SpriteMeshShader");
	_transformBuffer = FIND_CBUFFER("Transform", TransformCBuffer);
	_spriteBuffer = FIND_CBUFFER("Sprite", SpriteCBuffer);

	if (!_shader)
	{
		return false;
	}

	if (!_spriteBuffer || !_transformBuffer)
	{
		return false;
	}
	return true;
}

void LightRenderPass::Render(const std::vector<FRenderObjectKey>& visibleKeys,
	std::unordered_map<FRenderObjectKey, struct FRenderItem, FRenderObjectKeyHash>& renderItems,
	Ptr<class BlendState> lightBlend)
{
	_lightTarget->Begin(_clearColor);
	lightBlend->SetState();
	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);

	Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel();
	if (!level)
	{
		return;
	}

	for (const FRenderObjectKey& key : visibleKeys)
	{
		auto it = renderItems.find(key);
		if (it == renderItems.end())
		{
			continue;
		}

		Ptr<SceneComponent> comp = Lock<SceneComponent>(it->second._component);
		Ptr<Light2DComponent> light = Cast<SceneComponent, Light2DComponent>(comp);
		if (!light || !light->IsActive() || !light->IsEnable())
		{
			continue;
		}

		FLightRenderProxy proxy;
		if (!light->BuildLightRenderProxy(proxy))
		{
			continue;
		}

		FVector4D tint = proxy._tint;
		tint *= proxy._intensity;
		tint._w = proxy._tint._w;

		_spriteBuffer->SetTint(tint);
		_spriteBuffer->SetParallaxOffset(0.f, 0.f);
		_spriteBuffer->SetParallaxPivot(0.f, 0.f);
		_spriteBuffer->SetParallaxScale(1.f);
		_spriteBuffer->Update();

		_transformBuffer->SetWorldMatrix(proxy._world);
		_transformBuffer->SetViewMatrix(level->GetViewMatrix());
		_transformBuffer->SetProjMatrix(level->GetProjMatrix());
		_transformBuffer->Update();

		_shader->SetShader();
		proxy._texture->SetShader(0, SHADER_TYPE::PIXEL, proxy._textureIndex);
 		proxy._mesh->Render();
		proxy._texture->ResetShader(0, SHADER_TYPE::PIXEL);
	}
	lightBlend->ResetState();
}

void LightRenderPass::Destroy()
{
	DESTROY(_lightTarget);
}

ComPtr<ID3D11ShaderResourceView> LightRenderPass::GetLightSRV()
{
	return _lightTarget->GetSRV();
}

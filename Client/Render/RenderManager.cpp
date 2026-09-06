#include "pch.h"
#include "RenderManager.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "RenderStateManager.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/Shader.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/ConstantBuffer/SpriteCBuffer.h"
#include "../Shader/ConstantBuffer/CompositeDebugCBuffer.h"


#include "../Component/SceneComponent.h"
#include "../Collision/CollisionSystem.h"
#include "../Component/CameraComponent.h"
#include "../Component/Animation/SpriteComponent.h"
#include "../Object/Actor.h"
#include "../World/Level.h"
#include "../Core/Device.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Component/Light2DComponent.h"

#include "../Core/GameEngine.h"
#include "../World/World.h"

#include "SpriteBatch.h"
#include "LightRenderPass.h"
#include "RenderPass.h"
#include "../Object/Asset/Texture/Texture.h"

#include "../Editor/Interface/Interface.h"
#include "../Editor/EditEngine.h"
#include "../Editor/EditorUI/ViewportRenderTarget.h"
#include <algorithm>

bool RenderManager::Init()
{
	_renderGrid = New<TileGrid>();
	if (!_renderGrid->Init(4096.f))
	{
		return false;
	}

	_spriteBatch = New<SpriteBatch>();
	if (!_spriteBatch->Init())
	{
		return false;
	}

	CreateRenderLayer("Background"		, 0,		false,	true, "BackgroundLight");
	CreateRenderLayer("BackgroundLight"	, 2,		true,	false, "");
	CreateRenderLayer("Chunck"			, 4,		false,	false, "");
	CreateRenderLayer("Default"			, 5,		false,	false, "");		// 게임 플레이 레이어
	CreateRenderLayer("Foreground"		, 12,		false,	false, "");
	CreateRenderLayer("Effect"			, 15,		false,	false, "");
	CreateRenderLayer("UI"				, 1000,		false,	false, "");			// UI 레이어
	CreateRenderLayer("Collider"		, INT_MAX,	false,	false, "");	// 보조용 Collider 레이어
	

	_renderStateManager = New<RenderStateManager>();
	if (!_renderStateManager->Init())
	{
		return false;
	}

	_alphaBlends.push_back(_renderStateManager->FindRenderState<BlendState>("AlphaBlend"));
	_alphaBlends.push_back(_renderStateManager->FindRenderState<BlendState>("LightMaskAlphaBlend")); // LightBlend
	_alphaBlends.push_back(_renderStateManager->FindRenderState<BlendState>("FogBlend"));
	_alphaBlends.push_back(_renderStateManager->FindRenderState<BlendState>("ScenePassBlend"));
	_alphaBlends.push_back(_renderStateManager->FindRenderState<BlendState>("LightBlend"));

	_depthStencilState = _renderStateManager->FindRenderState<DepthStencilState>("DepthStencil");
	_resterizerState = _renderStateManager->FindRenderState<RasteriazeState>("ResterizerState");
	RefreshLayer();

	_compositeShader = FIND_SHADER("LightCompositeShader");
	return true;
}

void RenderManager::Tick(float deltaTime)
{
	FlushRemovals();

	if (_refreshLayer)
	{
		for (auto& [order, layer] : _layers)
		{
			const int testorder = order;
			auto subIt = layer._finder.begin();
			while (subIt != layer._finder.end())
			{
				Ptr<SceneComponent> renderComp = Lock<SceneComponent>(subIt->second);
				if (nullptr == renderComp)
				{
					subIt++;
					continue;
				}

				FRenderLayer* curLayer = FindLayer(renderComp->GetRenderLayerName());
				if (nullptr == curLayer)
				{
					subIt++;
					continue;
				}

				if (curLayer->_order == order)
				{
					subIt++;
					continue;
				}

				Ptr<IComponentOwner> owner = renderComp->GetOwner();
				if (nullptr == owner)
				{
					subIt++;
					continue;
				}

				std::pair<int32, int32> key = { owner->GetOwnerID() , renderComp->GetComponentID()};
				curLayer->_finder[key] = subIt->second;

				subIt = layer._finder.erase(subIt);
				layer._refreshRenders = true;
			}
		}
		_refreshLayer = false;
	}
}

void RenderManager::Render(float deltaTime)
{
	UpdateRenderGrid();

	_depthStencilState->SetState();
	_resterizerState->SetState();
	 
	std::vector<FRenderObjectKey> visibleKeys;
	Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel();
	if (level && level->GetMainCamera())
	{
		FLocalBound2D viewBound = level->GetMainCamera()->GetViewBound2D(0.f);
		_renderGrid->Query(viewBound, visibleKeys);
	}

	for (auto& [order, layer] : _layers)
	{
		if (layer._isLightLayer)
		{
			continue;
		}

		if (layer._receivesLight)
		{
			RenderLitLayer(layer, visibleKeys, deltaTime);
			continue;
		}
		RenderLayerFromGrid(layer._name, visibleKeys, deltaTime);
	}

	_depthStencilState->ResetState();
	_resterizerState->ResetState();
}

void RenderManager::RefreshLayer()
{
	_refreshLayer = true;
}

void RenderManager::SetZSort(bool sort)
{
	_sortZ = sort;
}

void RenderManager::AddRenderComponent(int32 actorID, Ptr<class SceneComponent> comp)
{
	FRenderLayer* layer = FindLayer(comp->GetRenderLayerName());
	if (nullptr == layer)
	{
		return;
	}

	layer->_finder[{actorID, comp->GetComponentID()}] = comp;
	layer->_refreshRenders = true;

	FRenderObjectKey key;
	key._actorID = actorID;
	key._compID = comp->GetComponentID();

	auto [itemIt, inserted] = _renderItems.try_emplace(key);
	if (inserted)
	{
		itemIt->second._component = comp;
		itemIt->second._layerName = layer->_name;
		itemIt->second._layerOrder = layer->_order;
	}
	_dirtyBoundKeys.insert(key);
}

void RenderManager::RemoveRenderComponent(const std::string& layerName, int32 actorID, int32 compID)
{
	std::tuple<std::string, int32, int32> key = { layerName, actorID, compID };
	_removeRenders.push_back(key);
}

void RenderManager::FlushRemovals()
{
	for (auto& [layerName, actorID, compID] : _removeRenders)
	{
		// 레이어 finder에서 제거 (조회 실패해도 아래 _renderItems 정리는 진행)
		auto foundIt = _layerFinders.find(layerName);
		if (_layerFinders.end() != foundIt)
		{
			auto itLayer = _layers.find(foundIt->second);
			if (_layers.end() != itLayer)
			{
				itLayer->second._finder.erase({ actorID, compID });
				itLayer->second._refreshRenders = true;
			}
		}

		FRenderObjectKey removeKey;
		removeKey._actorID = actorID;
		removeKey._compID = compID;

		_renderItems.erase(removeKey);
		_renderGrid->Unregister(removeKey);
		_dirtyBoundKeys.erase(removeKey);   // (선택) 잔여 dirty 키 정리
	}
	_removeRenders.clear();
}

void RenderManager::MarkRenderBoundDirty(int32 actorID, int32 compID)
{
	FRenderObjectKey key{ actorID, compID };
	if (_renderItems.count(key)) // 지정한 키와 일치하는 요소의 수를 찾습니다
	{
		_dirtyBoundKeys.insert(key);
	}

}

void RenderManager::CompositeSceneAndLight(	const ComPtr<ID3D11ShaderResourceView> SceneSRV,
											const ComPtr<ID3D11ShaderResourceView> LightSRV)
{	
	if (!_compositeShader)
	{
		return;
	}

	_compositeShader->SetShader();
	ID3D11ShaderResourceView* source[2] = {
		SceneSRV.Get(),
		LightSRV.Get()
	};

#ifdef _EDITOR
	Ptr<CompositeDebugCBuffer> debugCBuffer = FIND_CBUFFER("CompositeDebug", CompositeDebugCBuffer);
	debugCBuffer->SetShowLight(_showLight);
	debugCBuffer->SetShowScene(_showScene);
	debugCBuffer->Update();
#endif // _EDITOR

	CONTEXT->PSSetShaderResources(0, 2, source);
	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);

	CONTEXT->IASetInputLayout(nullptr);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->Draw(3, 0);

	ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
	CONTEXT->PSSetShaderResources(0, 2, nullSRVs);
}

void RenderManager::RenderLitLayer(FRenderLayer& layer, const std::vector<FRenderObjectKey>& visibleKeys, float deltaTime)
{
	layer._scenePass->Begin(layer.DiffuseClearColor);
	RenderLayerFromGrid(layer._name, visibleKeys, deltaTime);

	layer._lightPass->Begin(layer.LightClearColor);
	RenderLightLayer(layer._lightLayerName, visibleKeys);

#ifdef _EDITOR
	EditEngine::Instance().GetViewportRenderTarget()->SetViewTarget();
#else
	Device::Instance().SetSceneTarget();
#endif // _EDITOR
	Ptr<BlendState> AlphaBlend = GetAlphaBlend(eBlendMode::DEFAULT);
	AlphaBlend->SetState();
	CompositeSceneAndLight(layer._scenePass->GetSRV(), layer._lightPass->GetSRV());
	AlphaBlend->ResetState();
}

void RenderManager::UpdateRenderGrid()
{
	_unregisterByLayer.clear();

	for (auto it = _dirtyBoundKeys.begin(); it != _dirtyBoundKeys.end() ;)
	{
		auto ItemIt = _renderItems.find(*it);
		if (_renderItems.end() == ItemIt)
		{
			it = _dirtyBoundKeys.erase(it);
			continue;
		}

		FRenderItem& Item = ItemIt->second;
		Ptr<SceneComponent> comp = Lock<SceneComponent>(Item._component);
		if (!comp || !comp->IsActive())
		{
			if (Item._registerInGrid)
			{
				_renderGrid->Unregister(*it);
				Item._registerInGrid = false;
			}
			it = _dirtyBoundKeys.erase(it);
			continue;
		}
		comp->ConsumeRenderBoundDirty();

		FLocalBound2D bound;
		if (comp->BuildRenderBound2D(bound))
		{
			Item._bound = bound;
			Item._layerName = comp->GetRenderLayerName();
			_renderGrid->Register(*it, bound);
			Item._registerInGrid = true;
		}
		else
		{
			_unregisterByLayer[Item._layerName].push_back(*it);
		}
		it = _dirtyBoundKeys.erase(it);
	}

	for (auto& [key, item] : _renderItems)
	{
		if (!item._registerInGrid)
		{
			_unregisterByLayer[item._layerName].push_back(key);
		}
	}
}

void RenderManager::RenderLayerFromGrid(const std::string& layerName, const std::vector<FRenderObjectKey>& visibleKeys ,float deltaTime)
{
	std::vector<Ptr<SceneComponent>> comps;

	for (const FRenderObjectKey& key : visibleKeys)
	{
		auto it = _renderItems.find(key);
		if (it == _renderItems.end())
		{
			continue;
		}

		FRenderItem& item = it->second;
		if (item._layerName != layerName)
		{
			continue;
		}

		Ptr<SceneComponent> comp = Lock<SceneComponent>(item._component);
		if (!comp || !comp->IsActive() || !comp->IsEnable())
		{
			continue;
		}
		comps.push_back(comp);
	}


	auto unregIt = _unregisterByLayer.find(layerName);
	if (unregIt != _unregisterByLayer.end())
	{
		for (const FRenderObjectKey& key : unregIt->second)
		{
			auto it = _renderItems.find(key);
			if (it == _renderItems.end())
			{
				continue;
			}

			Ptr<SceneComponent> comp = Lock<SceneComponent>(it->second._component);
			if (!comp || !comp->IsActive() || !comp->IsEnable())
			{
				continue;
			}
			comps.push_back(comp);
		}
	}
	RenderComponentSequence(comps, deltaTime);
}

void RenderManager::RenderLightLayer(const std::string& lightlayerName, const std::vector<FRenderObjectKey>& visibleKeys)
{
	Ptr<BlendState> lightBlend = GetAlphaBlend(eBlendMode::LIGHT);
	lightBlend->SetState();

	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);

	Ptr<Level> level = GameEngine::Instance().GetWorld()->GetCurLevel();
	if (!level)
	{
		lightBlend->ResetState();
		return;
	}

	for (const FRenderObjectKey& key : visibleKeys)
	{
		auto it = _renderItems.find(key);
		if (it == _renderItems.end())
		{
			continue;
		}

		FRenderItem& item = it->second;
		if (item._layerName != lightlayerName)
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

		Ptr<Shader> shader = FIND_SHADER("SpriteMeshShader");
		Ptr<TransformCBuffer> transformBuffer = FIND_CBUFFER("Transform", TransformCBuffer);
		Ptr<SpriteCBuffer> spriteBuffer = FIND_CBUFFER("Sprite", SpriteCBuffer);
		spriteBuffer->SetTint(tint);
		spriteBuffer->SetParallaxOffset(0.f, 0.f);
		spriteBuffer->SetParallaxPivot(0.f, 0.f);
		spriteBuffer->SetParallaxScale(1.f);
		spriteBuffer->Update();

		transformBuffer->SetWorldMatrix(proxy._world);
		transformBuffer->SetViewMatrix(level->GetViewMatrix());
		transformBuffer->SetProjMatrix(level->GetProjMatrix());
		transformBuffer->Update();

		shader->SetShader();
		proxy._texture->SetShader(0, SHADER_TYPE::PIXEL, proxy._textureIndex);
		proxy._mesh->Render();
		proxy._texture->ResetShader(0, SHADER_TYPE::PIXEL);
	}
	lightBlend->ResetState();
}

void RenderManager::RenderComponentSequence(std::vector<Ptr<class SceneComponent>>& comps, float deltaTime)
{
	std::sort(comps.begin(), comps.end(),
		[](Ptr<SceneComponent> src, Ptr<SceneComponent> dest)
		{
			return src->GetWorldPosition()._z > dest->GetWorldPosition()._z;
		});

	std::vector<FSpriteBatchVertex>& verts = _spriteBatch->GetStagingVerts();
	SpriteBatchKey curKey;

	bool hasKey = false;

	auto Flush = [&]()
		{
			if (verts.empty())
			{
				return;
			}

			_spriteBatch->Flush(verts, curKey._texture, curKey._textureIndex, curKey._blend);
			verts.clear();
		};

	for (Ptr<SceneComponent> comp : comps)
	{
		Ptr<SpriteComponent> sprite = Cast<SceneComponent, SpriteComponent>(comp);
		if (IsSpriteBatchable(sprite))
		{
			SpriteBatchKey nextKey;
			nextKey._texture		= sprite->GetTexture();
			nextKey._textureIndex	= sprite->GetTextureIndex();
			nextKey._blend			= sprite->GetBlendMode();

			if (hasKey && nextKey != curKey)
			{
				Flush();
			}

			curKey = nextKey;
			hasKey = true;

			_spriteBatch->ExpandMesh(sprite, verts);
			continue;
		}

		Flush();
		eBlendMode mode = comp->GetBlendMode();
		Ptr<BlendState> blend = GetAlphaBlend(mode);
		if (blend)
		{
			blend->SetState();
		}
		comp->Render(deltaTime);
		if (blend)
		{
			blend->ResetState();
		}
	}
	Flush();
}

bool RenderManager::IsSpriteBatchable(Ptr<class SpriteComponent> comp) const
{
	if (!comp)
	{
		return false;
	}

	if (!comp->GetMesh() || !comp->GetTexture())
	{
		return false;
	}

	if (comp->GetAnimation())
	{
		return false;
	}

	if (comp->GetShaderName() != "SpriteMeshShader")
	{
		return false;
	}
	return true;
}

FRenderLayer* RenderManager::FindLayer(const std::string& name)
{
	auto it = _layerFinders.find(name);
	if (_layerFinders.end() == it)
	{
		return nullptr;
	}

	auto subIt = _layers.find(it->second);
	if (_layers.end() == subIt)
	{
		return nullptr;
	}
	return &(subIt->second);
}

void RenderManager::Destroy()
{
	DESTROY(_renderStateManager);
	DESTROY(_depthStencilState);
	DESTROY(_resterizerState);

	DESTROY(_spriteBatch);
	DESTROY(_renderGrid);
}

Ptr<class BlendState> RenderManager::GetAlphaBlend(eBlendMode eMode)
{
	uint32 mode = static_cast<uint32>(eMode);
	switch (eMode)
	{
	case eBlendMode::DEFAULT:
		return _alphaBlends[mode];
	case eBlendMode::LIGHT:
		return _alphaBlends[mode];
	case eBlendMode::FOG:
		return _alphaBlends[mode];
	case eBlendMode::SCENEBLEND:
		return _alphaBlends[mode];
	case eBlendMode::LIGHTEFFECT:
		return _alphaBlends[mode];
	case eBlendMode::END:	// 이 두개는 동일하게 nullptr 을 반환하므로 이렇게 묶어줘도 됨.
	default:
		return nullptr;
	}
}

Ptr<class DepthStencilState> RenderManager::GetDepthStencilState()
{
	return _depthStencilState;
}

void RenderManager::CreateRenderLayer(	const std::string& name,
										int32 order,
										bool isLightLayer,
										bool receivesLight,
										const std::string& lightLayerName,
										FVector4D DiffuseClearColor/* = FVector4D(0.f, 0.f, 0.f, 1.f)*/,
										FVector4D LightClearColor/*	= FVector4D(0.f, 0.f, 0.f, 1.f)*/
									)
{
	FRenderLayer* foundLayer = FindLayer(name);
	if (foundLayer)
	{
		return;
	}

	FRenderLayer layer;
	layer._name = name;
	layer._order = order;
	layer._isLightLayer = isLightLayer;
	layer._receivesLight = receivesLight;
	layer._lightLayerName = lightLayerName;

	if (receivesLight)
	{
		layer._scenePass = New<RenderPass>();
		layer._lightPass = New<RenderPass>();

		layer._scenePass->Init();
		layer._lightPass->Init();

		layer.DiffuseClearColor[0] = DiffuseClearColor._x;
		layer.DiffuseClearColor[1] = DiffuseClearColor._y;
		layer.DiffuseClearColor[2] = DiffuseClearColor._z;
		layer.DiffuseClearColor[3] = DiffuseClearColor._w;

		layer.LightClearColor[0] = LightClearColor._x;
		layer.LightClearColor[1] = LightClearColor._y;
		layer.LightClearColor[2] = LightClearColor._z;
		layer.LightClearColor[3] = LightClearColor._w;
	}

	_layerFinders[name] = order;
	_layers[order] = layer;
}

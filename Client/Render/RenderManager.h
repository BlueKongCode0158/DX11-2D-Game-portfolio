#pragma once
#include "../Common/Singleton.h"
#include "../Common/Info.h"

#include "TileGrid.h"

struct FRenderItem
{
	Weak<class SceneComponent> _component;
	std::string _layerName;
	int32 _layerOrder;
	FLocalBound2D _bound;
	bool _registerInGrid = false;
};

struct FRenderLayer
{
	std::string _name;
	int32 _order;

	std::vector<Weak<class SceneComponent>> _renders;
	std::map<std::pair<int32, int32>, Weak<class SceneComponent>> _finder;	// 탐색할 때 빠르게 지워주고 싶어서
	bool _refreshRenders;

	bool _isLightLayer = false;
	bool _receivesLight = false;
	std::string _lightLayerName;

	Ptr<class RenderPass> _scenePass;
	Ptr<class RenderPass> _lightPass;

	float DiffuseClearColor[4] = { 0.f, 0.f, 0.f, 1.f};
	float LightClearColor[4] = { 0.f, 0.f, 0.f, 1.f};
};

class RenderManager : public Singleton<RenderManager>
{
	DECLARE_SINGLETON(RenderManager);
public:
	bool Init();
	void Tick(float deltaTime);
	void Render(float deltaTime);
	void RefreshLayer();
	void SetZSort(bool sort);
	void AddRenderComponent(int32 actorID, Ptr<class SceneComponent> comp);
	void RemoveRenderComponent(const std::string& layerName, int32 actorID, int32 compID);
	void FlushRemovals();
	void MarkRenderBoundDirty(int32 actorID, int32 compID);
	//FRenderLayer* FindLayer(const std::string& name);
public:
	void CompositeSceneAndLight(const ComPtr<ID3D11ShaderResourceView> SceneSRV,
								const ComPtr<ID3D11ShaderResourceView> LightSRV);	// Diffuse + Light
	void RenderLitLayer(FRenderLayer& layer, const std::vector<FRenderObjectKey>& visibleKeys, float deltaTime);
public:
#ifdef _EDITOR
	void SetShowScene(bool show) 
	{ 
		_showScene = show;
	}
	void SetShowLight(bool show)
	{
		_showLight = show;
	}
	bool GetShowScene() const
	{
		return _showScene;
	}
	bool GetLightScene() const
	{
		return _showLight;
	}
#endif // _EDITOR

private:
	void UpdateRenderGrid();
	void RenderLayerFromGrid(const std::string& layerName, const std::vector<FRenderObjectKey>& visibleKeys , float deltaTime);
	void RenderLightLayer(const std::string& lightlayerName, const std::vector<FRenderObjectKey>& visibleKeys);
	void RenderComponentSequence(std::vector<Ptr<class SceneComponent>>& comps, float deltaTime);
	bool IsSpriteBatchable(Ptr<class SpriteComponent> comp) const;
public:
	virtual void Destroy() override;
public:
	Ptr<class BlendState> GetAlphaBlend(eBlendMode eMode);
	Ptr<class DepthStencilState> GetDepthStencilState();
private:
	void CreateRenderLayer(	const std::string& name,
							int32 order,
							bool isLightLayer,
							bool receivesLight,
							const std::string& lightLayerName,
							FVector4D DiffuseClearColor = FVector4D(0.f, 0.f, 0.f, 1.f),
							FVector4D LightClearColor = FVector4D(0.f, 0.f, 0.f, 1.f));
	FRenderLayer* FindLayer(const std::string& name);
private:
	std::map<int32, FRenderLayer> _layers;
	std::unordered_map<std::string, int32> _layerFinders;
	std::vector<std::tuple<std::string, int32, int32>> _removeRenders;
	bool _refreshLayer;
	bool _sortZ = true;
	Ptr<class RenderStateManager>		_renderStateManager;
	Ptr<class DepthStencilState>		_depthStencilState;
	Ptr<class RasteriazeState>			_resterizerState;
	std::vector<Ptr<class BlendState>>	_alphaBlends;
//private:	// renderLayer 인자로 넘김
//	float _transparentClear[4]	= { 0.f, 0.f, 0.f, 1.f };
//	float _lightClearColor[4]	= { 0.f, 0.f, 0.f, 1.f };
private:
	// 최적화용 컬링 
	Ptr<TileGrid>			_renderGrid;
	Ptr<class SpriteBatch>	_spriteBatch;
	std::unordered_map<FRenderObjectKey, FRenderItem, FRenderObjectKeyHash> _renderItems;
	std::unordered_map<std::string, std::vector<FRenderObjectKey>> _unregisterByLayer;
	std::unordered_set<FRenderObjectKey, FRenderObjectKeyHash> _dirtyBoundKeys;
private:
#ifdef _EDITOR
	bool _showScene = false;
	bool _showLight = false;
#endif // _EDITOR
private:
	Ptr<class Shader> _compositeShader;
};
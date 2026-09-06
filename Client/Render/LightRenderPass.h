#pragma once
#include "../Object/Object.h"
#include "TileGrid.h"

class LightRenderPass : public Object
{
public:
	LightRenderPass();
	virtual ~LightRenderPass();
public:
	bool Init();
	void Render(const std::vector<FRenderObjectKey>& visibleKeys,
		std::unordered_map<FRenderObjectKey, struct FRenderItem, 
		FRenderObjectKeyHash>& renderItems,
		Ptr<class BlendState> lightBlend);
	virtual void Destroy();
public:
	ComPtr<ID3D11ShaderResourceView> GetLightSRV();
private:
	float _clearColor[4]	= { 0.f, 0.f, 0.f, 1.f };
	Ptr<class RenderTarget2D>	_lightTarget;
	Ptr<class TransformCBuffer> _transformBuffer;
	Ptr<class SpriteCBuffer>	_spriteBuffer;
	Ptr<class Shader>			_shader;
};
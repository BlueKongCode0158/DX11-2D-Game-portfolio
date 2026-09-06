#pragma once
#include "../Object/Object.h"

class RenderStateManager : public Object
{
public:
	RenderStateManager();
	virtual ~RenderStateManager();
public:
	bool Init(); 
public:
	void SetBlendFactor(const std::string& name, float r, float g, float b, float a);
	void SetSampleMask(const std::string& name, UINT mask);
	void AddBlendDesc(const std::string& name, bool enable,
		D3D11_BLEND src, D3D11_BLEND dest, D3D11_BLEND_OP blendOp,
		D3D11_BLEND srcAlpha, D3D11_BLEND destAlpha,
		D3D11_BLEND_OP alphaOp, UINT8 renderTargetWriteMask);
	bool CreateState(const std::string& name, bool alphaToCoverage, bool Indentent);
	bool CreateDepthStencilState(const std::string& name, bool depthEnable = true,
		D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS,
		bool stencilEnable = false,
		UINT8 stencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
		UINT8 stencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
		D3D11_DEPTH_STENCILOP_DESC frontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS },
		D3D11_DEPTH_STENCILOP_DESC backFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS });
	bool CreateRasterizerState(
		const std::string& name,
		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE cullMode = D3D11_CULL_BACK,
		bool frontCounterClockwise = false,
		bool depthClipEnable = true
		);
	
	virtual void Destroy() override;
	template<typename T>
	Ptr<T> FindRenderState(const std::string& name)
	{
		auto it = _renderStates.find(name);
		if (_renderStates.end() == it)
		{
			return nullptr;
		}
		return Cast<RenderState, T>(it->second);
	}
private:
	std::unordered_map<std::string, Ptr<class RenderState>> _renderStates;
};
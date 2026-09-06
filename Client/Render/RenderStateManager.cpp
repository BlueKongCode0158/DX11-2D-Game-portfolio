#include "pch.h"
#include "RenderStateManager.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"

RenderStateManager::RenderStateManager()
{
}

RenderStateManager::~RenderStateManager()
{
}

bool RenderStateManager::Init()
{
	// 알파가 등록 안되고 있어서 
	// Scene 에서 안 그려졌던거임.
	AddBlendDesc("AlphaBlend",
		true,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL
	);

	AddBlendDesc("FogBlend",
		true,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL);

	AddBlendDesc("LightBlend",
		true,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL
	);

	AddBlendDesc("LightMaskAlphaBlend",
		true,
		D3D11_BLEND_ONE,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_RED |
		D3D11_COLOR_WRITE_ENABLE_GREEN |
		D3D11_COLOR_WRITE_ENABLE_BLUE
	);

	AddBlendDesc("ScenePassBlend",
		true,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL
	);

	CreateState("AlphaBlend", false, false);
	CreateState("FogBlend", false, false);
	CreateState("LightBlend", false, false);
	CreateState("LightMaskAlphaBlend", false, false);
	CreateState("ScenePassBlend", false, false);

	CreateDepthStencilState("DepthStencil", false);
	CreateRasterizerState("ResterizerState", D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true);
	return true;
}

void RenderStateManager::SetBlendFactor(const std::string& name, float r, float g, float b, float a)
{
	Ptr<BlendState> state = FindRenderState<BlendState>(name);
	if (nullptr == state)
	{
		state = New<BlendState>();
		_renderStates[name] = state;
	}
	state->SetBlendFactor(r, g, b, a);
}

void RenderStateManager::SetSampleMask(const std::string& name, UINT mask)
{
	Ptr<BlendState> state = FindRenderState<BlendState>(name);
	if (nullptr == state)
	{
		state = New<BlendState>();
		_renderStates[name] = state;
	}
	state->SetSampleMask(mask);
}

void RenderStateManager::AddBlendDesc(const std::string& name, bool enable, D3D11_BLEND src, D3D11_BLEND dest, D3D11_BLEND_OP blendOp, D3D11_BLEND srcAlpha, D3D11_BLEND destAlpha, D3D11_BLEND_OP alphaOp, UINT8 renderTargetWriteMask)
{
	Ptr<BlendState> state = FindRenderState<BlendState>(name);
	if (nullptr == state)
	{
		state = New<BlendState>();
		_renderStates[name] = state;
	}
	state->AddBlendDesc(enable, src, dest, blendOp, srcAlpha, destAlpha, alphaOp, renderTargetWriteMask);
}


bool RenderStateManager::CreateState(const std::string& name, bool alphaToCoverage, bool Indentent)
{
	Ptr<BlendState> state = FindRenderState<BlendState>(name);
	if (nullptr == state)
	{
		state = New<BlendState>();
		_renderStates[name] = state;
	}
	if (!state->CreateState(alphaToCoverage, Indentent))
	{
		DESTROY(state);
		return false;
	}
	return true;
}

bool RenderStateManager::CreateDepthStencilState(const std::string& name, bool depthEnable, D3D11_DEPTH_WRITE_MASK depthWriteMask, D3D11_COMPARISON_FUNC depthFunc, bool stencilEnable, UINT8 stencilReadMask, UINT8 stencilWriteMask, D3D11_DEPTH_STENCILOP_DESC frontFace, D3D11_DEPTH_STENCILOP_DESC backFace)
{
	Ptr<DepthStencilState> state = FindRenderState<DepthStencilState>(name);
	if (nullptr != state)
	{
		return false;
	}
	state = New<DepthStencilState>();
	if (!state->CreateState(depthEnable, depthWriteMask, depthFunc, stencilEnable, stencilReadMask, stencilWriteMask, frontFace, backFace))
	{
		DESTROY(state);
		return false;
	}
	_renderStates[name] = state;
	return true;
}

bool RenderStateManager::CreateRasterizerState(const std::string& name, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool frontCounterClockwise, bool depthClipEnable)
{
	Ptr<RasteriazeState> state = FindRenderState<RasteriazeState>(name);
	if (nullptr != state)
	{
		return false;
	}
	state = New<RasteriazeState>();
	if (!state->CreateState(fillMode, cullMode, frontCounterClockwise, depthClipEnable))
	{
		return false;
	}
	_renderStates[name] = state;
	return true;
}

void RenderStateManager::Destroy()
{
	for (auto& it : _renderStates)
	{
		DESTROY(it.second);
	}
}

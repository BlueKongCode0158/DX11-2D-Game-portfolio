#include "pch.h"
#include "RasterizerState.h"
#include "../Core/Device.h"

RasteriazeState::RasteriazeState()
{
}

RasteriazeState::~RasteriazeState()
{
}

bool RasteriazeState::CreateState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool frontCounterClockwise, bool depthClipEnable)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = frontCounterClockwise;
	desc.DepthClipEnable = depthClipEnable;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;

	HRESULT hr = DEVICE->CreateRasterizerState(&desc, (ID3D11RasterizerState**)_state.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void RasteriazeState::SetState()
{
	CONTEXT->RSGetState((ID3D11RasterizerState**)_prevState.GetAddressOf());
	CONTEXT->RSSetState((ID3D11RasterizerState*)_state.Get());
}

void RasteriazeState::ResetState()
{
	CONTEXT->RSSetState((ID3D11RasterizerState*)_prevState.Get());
}

void RasteriazeState::Destroy()
{
}

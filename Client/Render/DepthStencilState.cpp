#include "pch.h"
#include "DepthStencilState.h"
#include "../Core/Device.h"

DepthStencilState::DepthStencilState()
{
	_type = eRenderState::RENDER_STATE_DEPTHSTENCIL;
}

DepthStencilState::~DepthStencilState()
{
}

bool DepthStencilState::CreateState(bool depthEnable, D3D11_DEPTH_WRITE_MASK depthWriteMask, D3D11_COMPARISON_FUNC depthFunc, bool stencilEnable, UINT8 stencilReadMask, UINT8 stencilWriteMask, D3D11_DEPTH_STENCILOP_DESC frontFace, D3D11_DEPTH_STENCILOP_DESC backFace)
{
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = depthEnable;
	desc.DepthWriteMask = depthWriteMask;
	desc.DepthFunc = depthFunc;
	desc.StencilEnable = stencilEnable;
	desc.StencilReadMask = stencilReadMask;
	desc.StencilWriteMask = stencilWriteMask;
	desc.FrontFace = frontFace;
	desc.BackFace = backFace;

	if (FAILED(DEVICE->CreateDepthStencilState(&desc, (ID3D11DepthStencilState**)_state.GetAddressOf())))
	{
		return false;
	}
	return true;
}

void DepthStencilState::SetState()
{
	Device::Instance().GetContext()->OMGetDepthStencilState((ID3D11DepthStencilState**)_prevState.GetAddressOf(), &_prevStencilRef);
	Device::Instance().GetContext()->OMSetDepthStencilState((ID3D11DepthStencilState*)_state.Get(), _stencilRef);
}

void DepthStencilState::ResetState()
{
	Device::Instance().GetContext()->OMSetDepthStencilState((ID3D11DepthStencilState*)_prevState.Get(), _prevStencilRef);
	_prevState = nullptr;

}

void DepthStencilState::Destroy()
{
}

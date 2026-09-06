#include "pch.h"
#include "BlendState.h"
#include "../Core/Device.h"
BlendState::BlendState()
{
	_type = eRenderState::RENDER_STATE_BLEND;
}

BlendState::~BlendState()
{
}

void BlendState::SetBlendFactor(float r, float g, float b, float a)
{
	_blendFactor[0] = r;
	_blendFactor[1] = g;
	_blendFactor[2] = b;
	_blendFactor[3] = a;
}

void BlendState::SetSampleMask(UINT mask)
{
	_sampleMask = mask;
}

void BlendState::AddBlendDesc(bool enable, D3D11_BLEND src, D3D11_BLEND dest, D3D11_BLEND_OP blendOp, D3D11_BLEND srcAlpha, D3D11_BLEND destAlpha, D3D11_BLEND_OP alphaOp, UINT8 renderTargetWriteMask)
{
	D3D11_RENDER_TARGET_BLEND_DESC desc = {};
	desc.BlendEnable = enable;
	desc.SrcBlend = src;
	desc.DestBlend = dest;
	desc.BlendOp = blendOp;
	desc.SrcBlendAlpha = srcAlpha;
	desc.DestBlendAlpha = destAlpha;
	desc.BlendOpAlpha = alphaOp;
	desc.RenderTargetWriteMask = renderTargetWriteMask;
	_desces.emplace_back(desc);
}

bool BlendState::CreateState(bool alphaToCoverage, bool Indendent)
{
	if (_desces.empty())
	{
		return false;
	}
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = alphaToCoverage;
	desc.IndependentBlendEnable = Indendent;
	for (int i = 0; i < 8; i++)
	{
		desc.RenderTarget[i].BlendEnable = false;
		desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	memcpy(desc.RenderTarget, &_desces[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC) * _desces.size());
	if (FAILED(DEVICE->CreateBlendState(&desc, (ID3D11BlendState**)_state.GetAddressOf())))
	{
		return false;
	}
	return true;
}

void BlendState::Destroy()
{
}

void BlendState::SetState()
{
	Device::Instance().GetContext()->OMGetBlendState((ID3D11BlendState**)_prevState.GetAddressOf(), _prevblendFactor, &_prevSampleMask);
	Device::Instance().GetContext()->OMSetBlendState((ID3D11BlendState*)_state.Get(), _blendFactor, _sampleMask);
}

void BlendState::ResetState()
{
	Device::Instance().GetContext()->OMSetBlendState((ID3D11BlendState*)_prevState.Get(), _prevblendFactor, _prevSampleMask);
	_prevState = nullptr;
}

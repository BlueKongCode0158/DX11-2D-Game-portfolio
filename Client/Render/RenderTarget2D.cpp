#include "pch.h"
#include "RenderTarget2D.h"
#include "../Core/Device.h"

RenderTarget2D::RenderTarget2D()
{
}

RenderTarget2D::~RenderTarget2D()
{
}

bool RenderTarget2D::Init()
{
	D3D11_TEXTURE2D_DESC desc = Device::Instance().GetBackBufferDesc();
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	if (FAILED(Device::Instance().GetDevice()->CreateTexture2D(&desc, nullptr, _texture.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(Device::Instance().GetDevice()->CreateRenderTargetView(_texture.Get(), nullptr, _renderTargetView.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(Device::Instance().GetDevice()->CreateShaderResourceView(_texture.Get(), nullptr, _shaderResourceView.GetAddressOf())))
	{
		return false;
	}
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(desc.Width);
	_viewport.Height = static_cast<float>(desc.Height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
	return true;
}

void RenderTarget2D::Begin(const float clearColor[4])
{
	// Bind + Clear 로 나눠도 됨.
	CONTEXT->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	CONTEXT->RSSetViewports(1, &_viewport);
	CONTEXT->ClearRenderTargetView(_renderTargetView.Get(), clearColor);
}

void RenderTarget2D::Destroy()
{
	_texture.Reset();
	_renderTargetView.Reset();
	_shaderResourceView.Reset();
}

ComPtr<ID3D11ShaderResourceView> RenderTarget2D::GetSRV() const
{
	return _shaderResourceView;
}

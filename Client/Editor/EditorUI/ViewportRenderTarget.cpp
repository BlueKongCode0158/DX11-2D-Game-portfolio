#include "pch.h"
#include "ViewportRenderTarget.h"
#include "../../Core/Device.h"
#include "../EditEngine.h"


ViewportRenderTarget::ViewportRenderTarget()
{
}

ViewportRenderTarget::~ViewportRenderTarget()
{
}

bool ViewportRenderTarget::Init(uint32 width, uint32 height)
{
	return CreateRenderTarget(width, height);
}

void ViewportRenderTarget::Resize(uint32 width, uint32 height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	if (width == _width && height == _height)
	{
		return;
	}

	ReleaseRenderTarget();
	CreateRenderTarget(width, height);
}

void ViewportRenderTarget::SetViewTarget()
{
	CONTEXT->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthView.Get());
	CONTEXT->RSSetViewports(1, &_viewport);

	Device::Instance().SetActiveRenderSource(_texture.Get());
}

void ViewportRenderTarget::Begin(const float clearColor[4])
{
	CONTEXT->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthView.Get());
	CONTEXT->ClearRenderTargetView(_renderTarget.Get(), clearColor);

	CONTEXT->ClearDepthStencilView(_depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	CONTEXT->RSSetViewports(1, &_viewport);

	Device::Instance().SetActiveRenderSource(_texture.Get());
}

void ViewportRenderTarget::End()
{
}

bool ViewportRenderTarget::CreateRenderTarget(uint32 width, uint32 height)
{
	_width = width;
	_height = height;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(DEVICE->CreateTexture2D(&texDesc, nullptr, _texture.GetAddressOf())))
	{
		return false;
	}
	if (FAILED(DEVICE->CreateRenderTargetView(_texture.Get(), nullptr, _renderTarget.GetAddressOf())))
	{
		return false;
	}
	if (FAILED(DEVICE->CreateShaderResourceView(_texture.Get(), nullptr, _resourceView.GetAddressOf())))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = width;
	depthDesc.Height = height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(DEVICE->CreateTexture2D(&depthDesc, nullptr, _depthTexture.GetAddressOf())))
	{
		return false;
	}
	if (FAILED(DEVICE->CreateDepthStencilView(_depthTexture.Get(), nullptr, _depthView.GetAddressOf())))
	{
		return false;
	}

	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftX = 0.f;

	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);

	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
	return true;
}

void ViewportRenderTarget::ReleaseRenderTarget()
{
	_depthView.Reset();
	_depthTexture.Reset();

	_resourceView.Reset();
	_renderTarget.Reset();
	_texture.Reset();
}

void ViewportRenderTarget::Destroy()
{
	ReleaseRenderTarget();
	_width = 0;
	_height = 0;
}
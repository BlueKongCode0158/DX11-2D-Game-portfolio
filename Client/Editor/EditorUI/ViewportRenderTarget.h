#pragma once
#include "../../Object/Object.h"

class ViewportRenderTarget : public Object
{
public:
	ViewportRenderTarget();
	virtual ~ViewportRenderTarget();
public:
	bool Init(uint32 width, uint32 height);
	void Resize(uint32 width, uint32 height);
	void SetViewTarget();
	void Begin(const float clearColor[4]);
	void End();
public:
	ComPtr<ID3D11ShaderResourceView> GetSRV() const
	{
		return _resourceView;
	}
	uint32 GetWidth() const
	{
		return _width;
	}
	uint32 GetHeight() const
	{
		return _height;
	}
private:
	bool CreateRenderTarget(uint32 width, uint32 height);
	void ReleaseRenderTarget();
public:
	virtual void Destroy() override;
private:
	uint32 _width = 0;
	uint32 _height = 0;
private:
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11RenderTargetView> _renderTarget;
	ComPtr<ID3D11ShaderResourceView> _resourceView;
private:
	ComPtr<ID3D11Texture2D> _depthTexture;
	ComPtr<ID3D11DepthStencilView> _depthView;
private:
	D3D11_VIEWPORT _viewport = {};
};
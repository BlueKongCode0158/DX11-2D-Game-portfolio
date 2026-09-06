#pragma once
#include "../Object/Object.h"

class RenderTarget2D : public Object
{
public:
	RenderTarget2D();
	virtual ~RenderTarget2D();
public:
	bool Init();
	void Begin(const float clearColor[4]);
	virtual void Destroy() override;
	ComPtr<ID3D11ShaderResourceView> GetSRV() const;
private:
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
	D3D11_VIEWPORT _viewport = {};
};
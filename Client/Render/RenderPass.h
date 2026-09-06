#pragma once
#include "../Object/Object.h"

class RenderPass : public Object
{
public:
	RenderPass();
	virtual ~RenderPass();
public:
	bool Init();
	void Begin(const float clearColor[4]);
	ComPtr<ID3D11ShaderResourceView> GetSRV() const;
public:
	virtual void Destroy() override;
private:
	Ptr<class RenderTarget2D> _target;
};
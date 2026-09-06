#pragma once
#include "RenderState.h"

class BlendState : public RenderState
{
public:
	BlendState();
	virtual ~BlendState();
public:
	void SetBlendFactor(float r, float g, float b, float a);
	void SetSampleMask(UINT mask);
	void AddBlendDesc(bool enable, D3D11_BLEND src,
		D3D11_BLEND dest, D3D11_BLEND_OP blendOp,
		D3D11_BLEND srcAlpha, D3D11_BLEND destAlpha,
		D3D11_BLEND_OP alphaOp, UINT8 renderTargetWriteMask);
	bool CreateState(bool alphaToCoverage, bool Indendent);
public:
	virtual void Destroy() override;
public:
	virtual void SetState() override;
	virtual void ResetState() override;
protected:
	std::vector<D3D11_RENDER_TARGET_BLEND_DESC> _desces;
	float _blendFactor[4] = {};
	float _prevblendFactor[4] = {};
	UINT _sampleMask = 0xffffffff;
	UINT _prevSampleMask = 0;
};
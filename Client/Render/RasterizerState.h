#pragma once
#include "RenderState.h"

class RasteriazeState : public RenderState
{
public:
	RasteriazeState();
	virtual ~RasteriazeState();
public:
	bool CreateState(
		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE cullMode = D3D11_CULL_BACK,
		bool frontCounterClockwise = false,
		bool depthClipEnable = true
	);
public:
	virtual void SetState() override;
	virtual void ResetState() override;
	virtual void Destroy() override;
};
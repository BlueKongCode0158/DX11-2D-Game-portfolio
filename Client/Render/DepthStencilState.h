#pragma once
#include "RenderState.h"

class DepthStencilState : public RenderState
{
public:
	DepthStencilState();
	virtual ~DepthStencilState();
public:
	bool CreateState(bool depthEnable = true,
		D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS,
		bool stencilEnable = false,
		UINT8 stencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
		UINT8 stencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
		D3D11_DEPTH_STENCILOP_DESC frontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS },
		D3D11_DEPTH_STENCILOP_DESC backFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS });
public:
	virtual void SetState() override;
	virtual void ResetState() override;
	virtual void Destroy() override;
protected:
	uint32 _stencilRef = 0xffffffff;
	uint32 _prevStencilRef = 0xffffffff;
};
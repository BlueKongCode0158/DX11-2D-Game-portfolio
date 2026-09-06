#pragma once
#include "../Object/Object.h"

class RenderState : public Object
{
public:
	RenderState() = default;
	virtual ~RenderState() = default;
public:
	virtual void SetState() = 0;
	virtual void ResetState() = 0;
protected:
	ComPtr<ID3D11DeviceChild> _state;
	ComPtr<ID3D11DeviceChild> _prevState;
	eRenderState _type = eRenderState::RENDER_STATE_END;
};
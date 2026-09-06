#include "pch.h"
#include "RenderPass.h"
#include "RenderTarget2D.h"

RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

bool RenderPass::Init()
{
    _target = New<RenderTarget2D>();
    
    if (!_target)
    {
        return false;
    }

    if (!_target->Init())
    {
        return false;
    }
    return true;
}

void RenderPass::Begin(const float clearColor[4])
{
    if (_target)
    {
        _target->Begin(clearColor);
    }
}

ComPtr<ID3D11ShaderResourceView> RenderPass::GetSRV() const
{
    if (!_target)
    {
        return nullptr;
    }
    return _target->GetSRV();
}

void RenderPass::Destroy()
{
    DESTROY(_target);
}

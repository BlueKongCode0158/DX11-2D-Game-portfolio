#include "pch.h"
#include "SpriteBatchShader.h"

SpriteBatchShader::SpriteBatchShader()
{
}

SpriteBatchShader::~SpriteBatchShader()
{
}

bool SpriteBatchShader::Init()
{
    if (!LoadVertexShader("SpriteBatchVS", "SpriteBatch.fx"))
    {
        return false;
    }
    if (!LoadPixelShader("SpriteBatchPS", "SpriteBatch.fx"))
    {
        return false;
    }

    AddInputLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
    AddInputLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0);
    AddInputLayoutDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0);
    
    if (!CreateInputLayout())
    {
        return false;
    }
    return true;
}

void SpriteBatchShader::Destroy()
{

}

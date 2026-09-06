#include "pch.h"
#include "LightCompositeShader.h"

LightCompositeShader::LightCompositeShader()
{
}

LightCompositeShader::~LightCompositeShader()
{
}

bool LightCompositeShader::Init()
{
    if (!LoadVertexShader("LightCompositeVS", "LightComposite.fx"))
    {
        return false;
    }

    if (!LoadPixelShader("LightCompositePS", "LightComposite.fx"))
    {
        return false;
    }
    return true;
}

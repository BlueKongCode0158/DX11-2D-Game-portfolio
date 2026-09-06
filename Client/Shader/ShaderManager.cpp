#include "pch.h"
#include "ConstantBuffer/TransformCBuffer.h"
#include "ConstantBuffer/ColorCBuffer.h"
#include "ConstantBuffer/MaterialCBuffer.h"
#include "ConstantBuffer/AnimCBuffer.h"
#include "ConstantBuffer/SpriteCBuffer.h"
#include "ConstantBuffer/FogPlaneCBuffer.h"
#include "ConstantBuffer/ParticleCBuffer.h"
#include "ConstantBuffer/CompositeDebugCBuffer.h"
#include "StructBuffer/ParticleSBuffer.h"
#include "../Common/LogManager.h"

#include "ShaderManager.h"
#include "ColorMeshShader.h"
#include "FrameMeshShader.h"
#include "MaterialPixelShader.h"
#include "StaticMeshShader.h"
#include "SpriteShader.h"
#include "SpriteMeshShader.h"
#include "FogShader.h"
#include "FogPlaneShader.h"
#include "SpriteBatchShader.h"
#include "ParticleShader.h"
#include "LightCompositeShader.h"

#include "../Core/Device.h"

void ShaderManager::CreateSampler()
{
    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    DEVICE->CreateSamplerState(&desc, _samplers[TEXTURE_SAMPLE_POINT].GetAddressOf());

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    DEVICE->CreateSamplerState(&desc, _samplers[TEXTURE_SAMPLE_LINEAR].GetAddressOf());

    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.MaxAnisotropy = 16;
    DEVICE->CreateSamplerState(&desc, _samplers[TEXTURE_SAMPLE_ANISOTROPIC].GetAddressOf());

    
}

bool ShaderManager::Init()
{
    if (false == CreateShader<ColorMeshShader>("ColorMeshShader"))
    {
        return false;
    }

    if (false == CreateShader<FrameMeshShader>("FrameMeshShader"))
    {
        return false;
    }

    if (false == CreateShader<StaticMeshShader>("StaticMeshShader"))
    {
        return false;
    }
    
    if (false == CreateShader<MaterialPixelShader>("MaterialPixelShader"))
    {
        LogManager::Instance().Debug("CreateShader<MaterialPixel> error");
        return false;
    }
    if (false == CreateShader<SpriteShader>("SpriteShader"))
    {
        LogManager::Instance().Debug("CreateShader<SpriteShader> error");
        return false;
    }
    if (false == CreateShader<SpriteMeshShader>("SpriteMeshShader"))
    {
        LogManager::Instance().Debug("CreateShader<SpriteMeshShader> error");
        return false;
    }
    if (false == CreateShader<FogShader>("FogShader"))
    {
        LogManager::Instance().Debug("CreateShader<FogShader> error");
        return false;
    }
    if (false == CreateShader<FogPlaneShader>("FogPlaneShader"))
    {
        LogManager::Instance().Debug("CreateShader<FogPlaneShader> error");
        return false;
    }
    if (false == CreateShader<SpriteBatchShader>("SpriteBatchShader"))
    {
        LogManager::Instance().Debug("CreateShader<SpriteBatchShader> error");
        return false;
    }
    if (false == CreateShader<ParticleShader>("ParticleShader"))
    {
        LogManager::Instance().Debug("CreateShader<ParticleShader> error");
        return false;
    }
    if (false == CreateShader<LightCompositeShader>("LightCompositeShader"))
    {
        LogManager::Instance().Debug("CreateShader<LightCompositeShader> error");
        return false;
    }


    if (false == CreateCBuffer<TransformCBuffer>("Transform", sizeof(FTransformCBufferData), 0, SHADER_TYPE::VERTEX))
    {
        return false;
    }

    if (false == CreateCBuffer<ColorCBuffer>("Color", sizeof(FColorCBufferData), 0, SHADER_TYPE::PIXEL))
    {
        return false;
    }
    
    if (false == CreateCBuffer<MaterialCBuffer>("Material", sizeof(FMaterialCBufferData), 1, SHADER_TYPE::PIXEL))
    {
        return false;
    }

    if (false == CreateCBuffer<SpriteCBuffer>("Sprite", sizeof(FSpriteCBufferData), 2, SHADER_TYPE::VERTEX | SHADER_TYPE::PIXEL))
    {
        return false;
    }

    if (false == CreateCBuffer<AnimCBuffer>("Anim", sizeof(FAnimationCBufferData), 1, SHADER_TYPE::VERTEX))
    {
        return false;
    }


    if (false == CreateCBuffer<FogPlaneCBuffer>("FogPlane", sizeof(FFogPlaneData), 3, SHADER_TYPE::PIXEL))
    {
        return false;
    }
    if (false == CreateCBuffer<ParticleCBuffer>("Particle", sizeof(FPartocleCBufferData), 1, SHADER_TYPE::PIXEL))
    {
        return false;
    }
    if (false == CreateCBuffer<CompositeDebugCBuffer>("CompositeDebug", sizeof(FCompisiteDebugData), 5, SHADER_TYPE::PIXEL))
    {
        return false;
    }
    if (false == CreateSBuffer<ParticleSBuffer>("Particle", sizeof(FParticleInstance), 100, 0, SHADER_TYPE::VERTEX))
    {
        return false;
    }

    CreateSampler();
    return true;
}

void ShaderManager::Destroy()
{
    for (auto& it : _shaders)
    {
        DESTROY(it.second);
    }

    for (auto& it : _cBuffers)
    {
        DESTROY(it.second);
    }
    _shaders.clear();
    _cBuffers.clear();
}

Ptr<Shader> ShaderManager::FindShader(const std::string& name)
{
    auto it = _shaders.find(name);
    if (_shaders.end() == it)
    {
        return nullptr;
    }

    return it->second;
}

void ShaderManager::SetSample(eTextureSampleType type)
{
    Device::Instance().GetContext()->PSSetSamplers(0, 1, _samplers[type].GetAddressOf());
}

#include "pch.h"
#include "MaterialPixelShader.h"
#include "../Core/Device.h"
#include "../Core/DirectoryManager.h"
#include "../Common/LogManager.h"

MaterialPixelShader::MaterialPixelShader()
{
}

MaterialPixelShader::~MaterialPixelShader()
{
}

bool MaterialPixelShader::Init()
{
    if (false == LoadPixelShader("DefaultMaterialPS", "Mesh.fx"))
    {
        return false;
    }
    return true;
}

void MaterialPixelShader::SetShader()
{
    Device::Instance().GetContext()->PSSetShader(_ps.Get(), nullptr, 0);
}

bool MaterialPixelShader::LoadPixelShader(const char* entryName, const char* fileName)
{
    auto hlslPath = DirectoryManager::Instance().GetCachePath("HLSL");
    if (!hlslPath.has_value())
    {
        return false;
    }

    std::filesystem::path fullPath;
    if (!DirectoryManager::Instance().GetFile(hlslPath.value(), fileName, OUT fullPath))
    {
        return false;
    }
    uint32 flag = 0;
#ifdef _DEBUG
    flag = D3DCOMPILE_DEBUG;
#endif // _DEBUG
    ID3D10Blob* errorBlob = nullptr;

    if (FAILED(D3DCompileFromFile(fullPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryName, "ps_5_0", flag , 0, _psBlob.GetAddressOf(), &errorBlob)))
    {
#ifdef _DEBUG
        char errorText[255] = {};
        strcpy_s(errorText, (const char*)errorBlob->GetBufferPointer());
        LogManager::Instance().Debug(errorText);
#endif // _DEBUG
        return false;
    }

    if (FAILED(Device::Instance().GetDevice()->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _ps.GetAddressOf())))
    {
        LogManager::Instance().Debug(__FILE__, __LINE__, "[CreatePixelShader Error");
        return false;
    }
    return true;
}

void MaterialPixelShader::Destroy()
{
}

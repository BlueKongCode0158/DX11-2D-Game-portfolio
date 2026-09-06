#include "pch.h"
#include "GraphicShader.h"
#include "Core/Device.h"
#include "Core/DirectoryManager.h"
#include "Common/LogManager.h"

GraphicShader::GraphicShader()
{
}

GraphicShader::~GraphicShader()
{
}

void GraphicShader::Destroy()
{
}

bool GraphicShader::Init()
{
	return true;
}

void GraphicShader::SetShader()
{
	Device::Instance().GetContext()->VSSetShader(_vs.Get(), nullptr, 0);
	Device::Instance().GetContext()->PSSetShader(_ps.Get(), nullptr, 0);
	Device::Instance().GetContext()->IASetInputLayout(_inputLayout.Get());
}

void GraphicShader::AddInputLayoutDesc(const char* semetic, uint32 sementicIndex, DXGI_FORMAT fmt, uint32 inputSlot, uint32 size, D3D11_INPUT_CLASSIFICATION inputSlotClass, uint32 instanceDatastepRate)
{
	D3D11_INPUT_ELEMENT_DESC desc = {};
	desc.SemanticName = semetic;
	desc.SemanticIndex = sementicIndex;
	desc.Format = fmt;
	desc.InputSlot = inputSlot;
	desc.InputSlotClass = inputSlotClass;
	desc.InstanceDataStepRate = instanceDatastepRate;
	desc.AlignedByteOffset = _inputLayoutSize;

	_inputLayoutSize += size;
	_descs.push_back(desc);
}

bool GraphicShader::CreateInputLayout()
{
	if (FAILED(Device::Instance().GetDevice()->CreateInputLayout(&_descs[0], (UINT)_descs.size(), _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), &_inputLayout)))
	{
		return false;
	}
	return true;
}

bool GraphicShader::LoadVertexShader(const char* entryName, const char* fileName)
{
	auto hlslPath = DirectoryManager::Instance().GetCachePath("HLSL");
	if (false == hlslPath.has_value())
	{
		return false;
	}
	std::filesystem::path fullpath;
	if (false == DirectoryManager::Instance().GetFile(hlslPath.value(), fileName, OUT fullpath))
	{
		return false;
	}

	uint32 flag = 0;
#ifdef _DEBUG
	flag = D3DCOMPILE_DEBUG;
#endif // DEBUG

	ID3D10Blob* errorBlob = nullptr;


	if (FAILED(D3DCompileFromFile(fullpath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryName, "vs_5_0", flag, 0, _vsBlob.GetAddressOf(), &errorBlob)))
	{
#ifdef _DEBUG
		char errorText[255] = {};
		strcpy_s(errorText, (const char*)errorBlob->GetBufferPointer());
		LogManager::Instance().Debug(errorText);
#endif // DEBUG
		return false;
	}

	if (FAILED(Device::Instance().GetDevice()->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vs.GetAddressOf())))
	{
		LogManager::Instance().Debug(__FILE__, __LINE__, "[CreateVertexShader Error]");
		return false;
	}

	return true;
}

bool GraphicShader::LoadPixelShader(const char* entryName, const char* fileName)
{
	auto hlslPath = DirectoryManager::Instance().GetCachePath("HLSL");
	if (false == hlslPath.has_value())
	{
		return false;
	}

	std::filesystem::path fullpath;
	if (false == DirectoryManager::Instance().GetFile(hlslPath.value(), fileName, OUT fullpath))
	{
		return false;
	}

	uint32 flag = 0;
#ifdef _DEBUG
	flag = D3DCOMPILE_DEBUG;
#endif // DEBUG

	ID3D10Blob* errorBlob = nullptr;


	if (FAILED(D3DCompileFromFile(fullpath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryName, "ps_5_0", flag, 0, _psBlob.GetAddressOf(), &errorBlob)))
	{
#ifdef _DEBUG
		char errorText[255] = {};
		strcpy_s(errorText, (const char*)errorBlob->GetBufferPointer());
		LogManager::Instance().Debug(errorText);
#endif // DEBUG
		return false;
	}

	if (FAILED(Device::Instance().GetDevice()->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _ps.GetAddressOf())))
	{
		LogManager::Instance().Debug(__FILE__, __LINE__, "[CreatePixelShader Error]");
		return false;
	}

	return true;
}

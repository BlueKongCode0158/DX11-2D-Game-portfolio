#include "pch.h"
#include "SBuffer.h"
#include "../../Core/Device.h"

SBuffer::SBuffer()
{
}

SBuffer::~SBuffer()
{
}

void SBuffer::Update()
{
}

bool SBuffer::Create(int32 size, int32 eleCon, int32 regi, int32 type)
{
	_size = size;
	_elementCount = eleCon;
	_register = regi;
	_type = type;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = size * _elementCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = _size;

	if (FAILED(Device::Instance().GetDevice()->CreateBuffer(&desc, nullptr, _buffer.GetAddressOf())))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = _elementCount;

	if (FAILED(Device::Instance().GetDevice()->CreateShaderResourceView(_buffer.Get(), &srvDesc, _srv.GetAddressOf())))
	{
		return false;
	}
	return true;
}

void SBuffer::Bind()
{
	if (_type & SHADER_TYPE::VERTEX)
	{
		Device::Instance().GetContext()->VSSetShaderResources(_register, 1, _srv.GetAddressOf());
	}
	if (_type & SHADER_TYPE::PIXEL)
	{
		Device::Instance().GetContext()->PSSetShaderResources(_register, 1, _srv.GetAddressOf());
	}
}

void SBuffer::Resize(int32 eleCnt)
{
	if (_elementCount >= eleCnt)
	{
		return;
	}

	_buffer.Reset();
	_srv.Reset();

	_elementCount = eleCnt;
	Create(_size, _elementCount, _register, _type);
}

void SBuffer::SetData(void* data, int32 eleCnt)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Device::Instance().GetContext()->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, _size * eleCnt);
	Device::Instance().GetContext()->Unmap(_buffer.Get(), 0);
}

void SBuffer::Destroy()
{
	
}

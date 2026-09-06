#include "pch.h"
#include "CBuffer.h"
#include "Core/Device.h"

CBuffer::CBuffer()
{
}

CBuffer::~CBuffer()
{
}

bool CBuffer::Create(int32 size, int32 _regi, int32 type)
{
    _size = size;
    _register = _regi;
    _type = type;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;

    desc.ByteWidth = _size;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(DEVICE->CreateBuffer(&desc, nullptr, _buffer.GetAddressOf())))
    {
        return false;
    }
    return true;
}

void CBuffer::SetData(void* data)
{
    D3D11_MAPPED_SUBRESOURCE map = {};
    CONTEXT->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    memcpy(map.pData, data, _size);

    CONTEXT->Unmap(_buffer.Get(), 0);
    // 이 enum은 2^n이어야겠네요.
    if (_type & SHADER_TYPE::VERTEX)
    {
        /* _register, 1 << Buffer Nums >>, buffer의 주소  */
        CONTEXT->VSSetConstantBuffers(_register, 1, _buffer.GetAddressOf());
    }
    if (_type & SHADER_TYPE::PIXEL)
    {
        assert(_buffer.Get() != nullptr);
        CONTEXT->PSSetConstantBuffers(_register, 1, _buffer.GetAddressOf());
    }
}

void CBuffer::Update()
{
}

void CBuffer::Destroy()
{
}

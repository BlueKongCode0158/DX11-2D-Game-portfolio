#include "pch.h"
#include "Asset/Mesh/Mesh.h"
#include "Core/Device.h"
#include "../Material/Material.h"
#include "../AssetManager.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}


bool Mesh::CreateMesh(const void* vertexData, int32 size, int32 count, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive, const void* indexData, int32 indexSize, int32 indexCount, DXGI_FORMAT fmt, D3D11_USAGE indexUsage)
{
    _vertextBuffer._size = size;
    _vertextBuffer._count = count;
    _vertextBuffer._data.resize(size * count);
    memcpy(_vertextBuffer._data.data(), vertexData, size * count);

    FLocalBound2D localBound;
    const byte* bytes = static_cast<const byte*>(vertexData);

    for (int32 i = 0; i < count; i++)
    {
        const FVector3D* pos = reinterpret_cast<const FVector3D*>(bytes + size * i);
        localBound.Expand(pos->_x, pos->_y);
    }

    if (localBound.IsValid())
    {
        SetLocalBound(localBound);
    }

    if (false == CreateBuffer(_vertextBuffer._buffer, D3D11_BIND_VERTEX_BUFFER, vertexData, size, count, vertexUsage))
    {
        return false;
    }
    _primitive = primitive;

    if (indexData != nullptr)
    {
        FMeshSlot slot;
        slot._indexBuffer._count = indexCount;
        slot._indexBuffer._data.resize(indexSize * indexCount);
        slot._indexBuffer._fmt = fmt;
        memcpy(slot._indexBuffer._data.data(), indexData, indexSize * indexCount);

        if (false == CreateBuffer(slot._indexBuffer._buffer, D3D11_BIND_INDEX_BUFFER, indexData, indexSize, indexCount, indexUsage))
        {
            return false;
        }
        slot._material = MATERIAL_MANAGER->CreateMaterialInstance("DefaultMaterial");
        _meshSlots.push_back(slot);
    }
    
    return true;
}


bool Mesh::CreateBuffer(ComPtr<ID3D11Buffer>& buffer, D3D11_BIND_FLAG flag, const void* data, int32 size, int32 count, D3D11_USAGE usage)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = size * count;
    bufferDesc.Usage = usage;
    bufferDesc.BindFlags = flag;
    bufferDesc.StructureByteStride = size;

    if (usage == D3D11_USAGE_DYNAMIC)
    {
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if (usage == D3D11_USAGE_STAGING)
    {
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    }

    D3D11_SUBRESOURCE_DATA bufferData = {};
    bufferData.pSysMem = data;

    if (FAILED(Device::Instance().GetDevice()->CreateBuffer(&bufferDesc, &bufferData, buffer.GetAddressOf())))
    {
        return false;
    }
    
    return true;
}

bool Mesh::SetLocalBound(FLocalBound2D localBound2D)
{
    return SetLocalBound(localBound2D._Min, localBound2D._Max);
}

bool Mesh::SetLocalBound(FVector2D& min, FVector2D& max)
{
    return SetLocalBound(min._x, min._y, max._x, max._y);
}

bool Mesh::SetLocalBound(float minX, float minY, float maxX, float maxY)
{
    _localBound._Min._x = minX;
    _localBound._Min._y = minY;
    _localBound._Max._x = maxX;
    _localBound._Max._y = maxY;
    return true;
}

void Mesh::Render()
{
    uint32 stride = _vertextBuffer._size;
    uint32 offset = 0;

    Device::Instance().GetContext()->IASetPrimitiveTopology(_primitive);
    Device::Instance().GetContext()->IASetVertexBuffers(0, 1, _vertextBuffer._buffer.GetAddressOf(), &stride, &offset);
    
    size_t size = _meshSlots.size();

    if (size > 0)
    {
        for (size_t i = 0; i < size; i++)
        {
            Device::Instance().GetContext()->IASetIndexBuffer(_meshSlots[i]._indexBuffer._buffer.Get(), _meshSlots[i]._indexBuffer._fmt, 0);
            Device::Instance().GetContext()->DrawIndexed(_meshSlots[i]._indexBuffer._count, 0, 0);
        }
    }
    else
    {
        Device::Instance().GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        Device::Instance().GetContext()->Draw(_vertextBuffer._count, 0);
    }
}

void Mesh::RenderInstanced(int32 instanceCount)
{
    uint32 stride = _vertextBuffer._size;
    uint32 offset = 0;

    CONTEXT->IASetPrimitiveTopology(_primitive);
    CONTEXT->IASetVertexBuffers(0, 1, _vertextBuffer._buffer.GetAddressOf(), &stride, &offset);

    size_t size = _meshSlots.size();
    for (size_t i = 0; i < size; i++)
    {
        CONTEXT->IASetIndexBuffer(_meshSlots[i]._indexBuffer._buffer.Get(),
            _meshSlots[i]._indexBuffer._fmt, 0);
        CONTEXT->DrawIndexedInstanced(_meshSlots[i]._indexBuffer._count, instanceCount, 0, 0, 0);
    }
}

void Mesh::Destroy()
{
}

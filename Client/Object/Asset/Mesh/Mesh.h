#pragma once
#include "Asset.h"

struct FMeshSlot
{
	FIndexBuffer _indexBuffer;
	Ptr<class Material> _material;
};


class Mesh : public Asset
{
public:
	Mesh();
	virtual ~Mesh();

public:
	bool CreateMesh(const void* vertexData, int32 size, int32 count, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive,
		const void* indexData, int32 indexSize, int32 indexCount, DXGI_FORMAT fmt, D3D11_USAGE indexUsage = D3D11_USAGE_DEFAULT);
	bool CreateBuffer(ComPtr<ID3D11Buffer>& buffer, D3D11_BIND_FLAG flag, const void* data, int32 size, int32 count,
		D3D11_USAGE usage);
	bool SetLocalBound(FLocalBound2D localBound2D);
	bool SetLocalBound(FVector2D& min, FVector2D& max);
	bool SetLocalBound(float minX, float minY, float maxX, float maxY);
	FLocalBound2D GetLocalBound2D()
	{
		return _localBound;
	}
	void Render();
	void RenderInstanced(int32 instanceCount);
public:
	size_t GetSlotCount()
	{
		return _meshSlots.size();
	}

	const FMeshSlot* GetSlot(int idx)
	{
		if (idx < 0 || idx >= _meshSlots.size())
		{
			return nullptr;
		}

		return &_meshSlots[idx];
	}

	FLocalBound2D& GetLocalBound()
	{
		return _localBound;
	}
	// Asset을(를) 통해 상속됨
	virtual void Destroy() override;
public:
	const std::vector<byte>& GetVertexRawData() const
	{
		return _vertextBuffer._data;
	}
	int32 GetVectexStride() const
	{
		return _vertextBuffer._size;
	}
	int32 GetVertexCount() const
	{
		return _vertextBuffer._count;
	}
	const std::vector<byte>& GetIndexRawData(int32 slot = 0) const
	{
		return _meshSlots[slot]._indexBuffer._data;
	}
	DXGI_FORMAT GetIndexFormat(int32 slot = 0) const
	{
		return _meshSlots[slot]._indexBuffer._fmt;
	}
	int32 GetIndexCount(int32 slot = 0) const
	{
		return _meshSlots[slot]._indexBuffer._count;
	}
protected:
	FVertexBuffer			_vertextBuffer;
	std::vector<FMeshSlot>	_meshSlots;
	D3D_PRIMITIVE_TOPOLOGY	_primitive;
	FLocalBound2D			_localBound;
};


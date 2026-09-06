#pragma once
#include "SBuffer.h"

// 자료담는 구조가 public이었으면
// SBuffer<T> 이런 형식이었으면 하나로 해결 할 수 있었다는 소리일까?
class TileLineSBuffer : public SBuffer
{
public:
	TileLineSBuffer();
	virtual ~TileLineSBuffer();
private:
	std::vector<FTileLineInstanceData> _datas;
public:
	virtual void Update() override;
	void AddData(const FVector2D& pos, FVector4D& color, const FVector2D& size)
	{
		FTileLineInstanceData data;
		data._worldPos = pos;
		data._color = color;
		data._size = size;

		_datas.push_back(data);
	}
	void Clear()
	{
		_datas.clear();
	}

	int32 GetCount()
	{
		return static_cast<int32>(_datas.size());
	}
};
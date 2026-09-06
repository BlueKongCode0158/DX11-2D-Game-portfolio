#include "pch.h"
#include "TileLineSBuffer.h"

TileLineSBuffer::TileLineSBuffer()
{
}

TileLineSBuffer::~TileLineSBuffer()
{
}

void TileLineSBuffer::Update()
{
	if (_datas.empty())
	{
		return;
	}

	int32 count = GetCount();
	Resize(count);
	SetData(_datas.data(), count);
}

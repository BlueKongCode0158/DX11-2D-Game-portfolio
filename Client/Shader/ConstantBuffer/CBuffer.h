#pragma once
#include "Object/Object.h"
#include "../CBufferData.h"

class CBuffer : public Object
{
public:
	CBuffer();
	virtual ~CBuffer();
public:
	bool Create(int32 size, int32 _regi, int32 type);
	void SetData(void* data);
public:
	virtual void Update() = 0;
	virtual void Destroy() override;
protected:
	ComPtr<ID3D11Buffer> _buffer = nullptr;
	int32 _size = 0;
	int32 _register = 0;
	int32 _type = 0;
};


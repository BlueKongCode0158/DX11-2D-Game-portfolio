#pragma once
#include "../../Object/Object.h"
#include "SBufferData.h"
// 구조화 버퍼
// 용량이 큰 버퍼를 보낼 때 사용한다 -> 지금까지 쓴 것 중엔 texture가 존재
// gpu에 대용량 데이터를 보낼 때 사용
// b0, b1

// 상수버퍼는 총 용량이 64kb
// 16 바이트 맞춤

class SBuffer : public Object
{
public:
	SBuffer();
	virtual ~SBuffer();
protected:
	ComPtr<ID3D11Buffer> _buffer;
	ComPtr<ID3D11ShaderResourceView> _srv; // resourceView
	int32 _size = 0;
	int32 _elementCount = 0;
	int32 _register = 0;
	int32 _type = 0;
public:
	virtual void Update() = 0;
	bool Create(int32 size, int32 eleCon, int32 regi, int32 type);
	void Bind();
protected:
	void SetData(void* data, int32 eleCnt);
	void Resize(int32 eleCnt);
public:
	virtual void Destroy() override;
};
#pragma once
#include "CBuffer.h"

class ParticleCBuffer : public CBuffer
{
public:
	ParticleCBuffer();
	virtual ~ParticleCBuffer();
public:
	virtual void Update() override;
	void SetUseTexture(bool use)
	{
		_data._useTexture = use ? 1 : 0;
	}
private:
	FPartocleCBufferData _data;
};
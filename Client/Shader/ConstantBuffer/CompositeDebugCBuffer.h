#pragma once
#include "CBuffer.h"
#include "../CBufferData.h"

class CompositeDebugCBuffer : public CBuffer
{
public:
	CompositeDebugCBuffer();
	virtual ~CompositeDebugCBuffer();
private:
	FCompisiteDebugData _data;
public:
	virtual void Update() override;
	void SetShowScene(bool show)
	{
		_data._showScene = show;
	}
	void SetShowLight(bool show)
	{
		_data._showLight = show;
	}
};
#include "pch.h"
#include "CompositeDebugCBuffer.h"

CompositeDebugCBuffer::CompositeDebugCBuffer()
{
}

CompositeDebugCBuffer::~CompositeDebugCBuffer()
{
}

void CompositeDebugCBuffer::Update()
{
	SetData(&_data);
}

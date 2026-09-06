#include "pch.h"
#include "UICBuffer.h"
#include "../CBufferData.h"

UICBuffer::UICBuffer()
{
}

UICBuffer::~UICBuffer()
{
}

void UICBuffer::Update()
{
	SetData(&_data);
}

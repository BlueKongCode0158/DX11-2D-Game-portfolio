#include "pch.h"
#include "ProgressBarCBuffer.h"

ProgressBarCBuffer::ProgressBarCBuffer()
{
}

ProgressBarCBuffer::~ProgressBarCBuffer()
{
}

void ProgressBarCBuffer::Update()
{
	SetData(&_data);
}

#include "pch.h"
#include "FogPlaneCBuffer.h"

FogPlaneCBuffer::FogPlaneCBuffer()
{
}

FogPlaneCBuffer::~FogPlaneCBuffer()
{
}

void FogPlaneCBuffer::Update()
{
	SetData(&_data);
}

#include "pch.h"
#include "ParticleCBuffer.h"

ParticleCBuffer::ParticleCBuffer()
{
}

ParticleCBuffer::~ParticleCBuffer()
{
}

void ParticleCBuffer::Update()
{
	SetData(&_data);
}
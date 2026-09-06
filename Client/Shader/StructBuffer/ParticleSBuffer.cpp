#include "pch.h"
#include "ParticleSBuffer.h"

ParticleSBuffer::ParticleSBuffer()
{
}

ParticleSBuffer::~ParticleSBuffer()
{
}

void ParticleSBuffer::Update()
{
	if (_datas.empty())
	{
		return;
	}

	int32 count = GetCount();
	Resize(count);
	SetData(_datas.data(), count);
}

void ParticleSBuffer::AddInstance(const FParticleInstance& inst)
{
	_datas.push_back(inst);
}

void ParticleSBuffer::Clear()
{
	_datas.clear();
}

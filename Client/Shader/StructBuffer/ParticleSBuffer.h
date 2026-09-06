#pragma once
#include "SBuffer.h"

class ParticleSBuffer : public SBuffer
{
public:
	ParticleSBuffer();
	virtual ~ParticleSBuffer();
public:
	virtual void Update() override;
	void AddInstance(const FParticleInstance& inst);
	void Clear();
public:
	int32 GetCount() const
	{
		return static_cast<int32>(_datas.size());
	}
private:
	std::vector<FParticleInstance> _datas;
};
#pragma once
#include "Object.h"

class SubManager : public Object
{
public:
	SubManager();
	virtual ~SubManager();
public:
	virtual bool Init() { return false; }

	// Object을(를) 통해 상속됨
	void Destroy() override;
};


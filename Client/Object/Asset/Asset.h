#pragma once
#include "Object.h"

class Asset : public Object
{
public:
	Asset();
	virtual ~Asset();
protected:
	std::string _name = "";
public:
	void SetName(const std::string& name)
	{
		this->_name = name;
	}

	const std::string& GetName() const
	{
		return _name;
	}

public:
	// Object을(를) 통해 상속됨
	void Destroy() override;
};


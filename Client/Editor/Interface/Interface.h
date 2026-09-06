#pragma once
#include <filesystem>
#include "../../Core/Defines.h"


class IArchive
{
public:
	IArchive() {};
	virtual ~IArchive() {};
public:
	virtual bool Save(std::ofstream& file) = 0;
	virtual bool Load(std::ifstream& file) = 0;
};

class IComponentOwner
{
public:
	virtual ~IComponentOwner() = default;
	virtual Ptr<class Level> GetLevel()		= 0;
	virtual int32			 GetOwnerID()	= 0;
};
#include "pch.h"
#include "TagManager.h"

bool TagManager::Init()
{
	Clear();
	return true;
}

void TagManager::Add(const std::string& tag, int32 id)
{
	_actorFinder[tag].insert(id);
}

void TagManager::Erase(const std::string& tag, int32 id)
{
	auto it = _actorFinder.find(tag);
	if (_actorFinder.end() == it)
	{
		return;
	}
	auto& itVal = it->second;
	itVal.erase(id);
}

void TagManager::Clear()
{
	if (_actorFinder.size() < 1)
	{
		return;
	}

	for (auto& it : _actorFinder)
	{
		if (it.second.size() < 1)
		{
			continue;
		}
		it.second.clear();
	}
	_actorFinder.clear();
}

void TagManager::GetActorIDs(const std::string& tag, OUT std::vector<int32>& OutArr)
{
	auto it = _actorFinder.find(tag);
	if (_actorFinder.end() == it)
	{
		return;
	}

	if (it->second.size() < 1)
	{
		return;
	}

	for (auto& subIt : it->second)
	{
		OutArr.push_back(subIt);
	}
}

void TagManager::Destroy()
{
}

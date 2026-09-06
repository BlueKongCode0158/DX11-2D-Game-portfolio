#pragma once
#include "Object/SubManager.h"
#include <set>

class TagManager : public SubManager
{
public:
	TagManager() = default;
	virtual ~TagManager() = default;
	TagManager(const TagManager&) = delete;
	TagManager(TagManager&&) = delete;
	TagManager& operator = (const TagManager&) = delete;
	TagManager& operator = (TagManager&&) = delete;
public:
	bool Init();
	void Add(const std::string& tag, int32 id);
	void Erase(const std::string& tag, int32 id);
	void Clear();
	void GetActorIDs(const std::string& tag, OUT std::vector<int32>& OutArr);
public:
	virtual void Destroy() override;
protected:
	std::unordered_map<std::string, std::set<int32>> _actorFinder;
};
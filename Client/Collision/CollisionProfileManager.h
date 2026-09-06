#pragma once
#include "Common/Singleton.h"
#include "Collision/CollisionProfile.h"

class CollisionProfileManager : public Singleton<CollisionProfileManager>
{
	DECLARE_SINGLETON(CollisionProfileManager);
private:
	std::map<eCollisionChannel, eCollisionResponse> _channels;
	std::unordered_map <std::string, Ptr<class CollisionProfile>> _profiles;
public:
	bool Init();
	void CreateChannel(eCollisionChannel channel, eCollisionResponse response);
	void CreateProfile(const std::string& name, eCollisionChannel channel);
	void SetProfileResponse(const std::string& profileName, eCollisionChannel channel, eCollisionResponse response);
	Ptr<class CollisionProfile> FindProfile(const std::string& name);
	virtual void Destroy() override;
public:
	template<typename... Args>
	void SetProfileResponse(const std::string& name, std::pair<eCollisionChannel, eCollisionResponse> response, Args... args)
	{
		Ptr<CollisionProfile> profile = FindProfile(name);
		if (nullptr == profile)
		{
			return;
		}
		profile->SetProfileResponse(response, args...);
	}
};
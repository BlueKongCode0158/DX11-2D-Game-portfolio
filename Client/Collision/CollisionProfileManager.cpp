#include "pch.h"
#include "CollisionProfileManager.h"

bool CollisionProfileManager::Init()
{
	CreateChannel(COLLISION_CHANNEL_PLAYER, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_MONSTER, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_MONSTERATTACK, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_ITEM, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_BOSS, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_ENVIRONMENT, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_CAMERAZONE, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_PLAYERATTACK, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_GATE, COLLISION_RESPONSE_IGNORE);
	CreateChannel(COLLISION_CHANNEL_PLAYERDECTECTION, COLLISION_RESPONSE_IGNORE);

	CreateProfile("Player", COLLISION_CHANNEL_PLAYER);
	CreateProfile("PlayerAttack", COLLISION_CHANNEL_PLAYERATTACK);
	CreateProfile("Monster", COLLISION_CHANNEL_MONSTER);
	CreateProfile("Environment", COLLISION_CHANNEL_ENVIRONMENT);
	CreateProfile("CameraZone", COLLISION_CHANNEL_CAMERAZONE);
	CreateProfile("Gate", COLLISION_CHANNEL_GATE);
	CreateProfile("PlayerDetection", COLLISION_CHANNEL_PLAYERDECTECTION);

	SetProfileResponse("Player",
		std::make_pair(COLLISION_CHANNEL_MONSTER, COLLISION_RESPONSE_OVERRAP),		// 겹쳤을 때 데미지.
		std::make_pair(COLLISION_CHANNEL_MONSTERATTACK, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_ITEM, COLLISION_RESPONSE_BLOCK),			// Gio 등
		std::make_pair(COLLISION_CHANNEL_BOSS, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_ENVIRONMENT, COLLISION_RESPONSE_BLOCK),	// 풀과 부딪혔을 때 풀이 밀리는 효과.
		std::make_pair(COLLISION_CHANNEL_CAMERAZONE, COLLISION_RESPONSE_OVERRAP),	// 카메라 존과 겹쳤을 때 CameraLock
		std::make_pair(COLLISION_CHANNEL_GATE, COLLISION_RESPONSE_OVERRAP)
	);

	SetProfileResponse("Monster",
		std::make_pair(COLLISION_CHANNEL_PLAYER, COLLISION_RESPONSE_OVERRAP),		// 몬스터 Collider (정확히는 플레이어 반응체) 와 닿았을 때 플레이어 인식
		std::make_pair(COLLISION_CHANNEL_PLAYERATTACK, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_ENVIRONMENT, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_ENVIRONMENT, COLLISION_RESPONSE_BLOCK)
	);

	SetProfileResponse("Environment",
		std::make_pair(COLLISION_CHANNEL_PLAYER, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_MONSTER, COLLISION_RESPONSE_BLOCK),
		std::make_pair(COLLISION_CHANNEL_PLAYERATTACK, COLLISION_RESPONSE_BLOCK)
	);

	SetProfileResponse("CameraZone",
		std::make_pair(COLLISION_CHANNEL_PLAYER, COLLISION_RESPONSE_OVERRAP));

	SetProfileResponse("PlayerAttack",
		std::make_pair(COLLISION_CHANNEL_MONSTER, COLLISION_RESPONSE_OVERRAP),
		std::make_pair(COLLISION_CHANNEL_ENVIRONMENT, COLLISION_RESPONSE_BLOCK));

	SetProfileResponse("Gate",
		std::make_pair(COLLISION_CHANNEL_PLAYER, COLLISION_RESPONSE_OVERRAP));

	return true;
}

void CollisionProfileManager::CreateChannel(eCollisionChannel channel, eCollisionResponse response)
{
	if (_channels.end() != _channels.find(channel))
	{
		return;
	}
	_channels[channel] = response;
	return;
}

void CollisionProfileManager::CreateProfile(const std::string& name, eCollisionChannel channel)
{
	if (_channels.end() == _channels.find(channel))
	{
		return;
	}

	if (_profiles.end() != _profiles.find(name))
	{
		return;
	}

	Ptr<CollisionProfile> profile = New<CollisionProfile>();
	profile->_name = name;
	profile->_channel = channel;

	for (auto it : _channels)
	{
		profile->_response[it.first] = it.second;
	}
	_profiles[profile->_name] = profile;

}

void CollisionProfileManager::SetProfileResponse(const std::string& profileName, eCollisionChannel channel, eCollisionResponse response)
{
	Ptr<CollisionProfile> profile = FindProfile(profileName);
	if (nullptr == profile)
	{
		return;
	}

	profile->_response[channel] = response;
}

Ptr<class CollisionProfile> CollisionProfileManager::FindProfile(const std::string& name)
{
	auto it = _profiles.find(name);
	if (_profiles.end() == it)
	{
		return nullptr;
	}

	return it->second;
}

void CollisionProfileManager::Destroy()
{
	for (auto& it : _profiles)
	{
		DESTROY(it.second);
	}
	_profiles.clear();
	_channels.clear();
}

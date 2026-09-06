#pragma once	
#include "Object/Object.h"
#include "Common/Info.h"

class CollisionProfile : public Object
{
	friend class CollisionProfileManager;
public:
	CollisionProfile();
	virtual ~CollisionProfile();
public:
	const eCollisionChannel GetChannel() const;
	const std::string GetName() const;
	const eCollisionResponse GetResponse(eCollisionChannel channel);
public:
	void SetProfileResponse(std::pair<eCollisionChannel, eCollisionResponse> response)
	{
		_response[response.first] = response.second;
	}
	template <typename... Args>
	void SetProfileResponse(std::pair<eCollisionChannel, eCollisionResponse> response, Args ...args)
	{
		SetProfileResponse(response);
		(SetProfileResponse(args), ...);
	}
public:
	void SetProfileResponse()
	{
		// TODO : LOGIC NOTTING
	}
public:
	virtual void Destroy() override;
private:
	std::string _name = "";
	eCollisionChannel _channel = eCollisionChannel::COLLISION_CHANNEL_END;
	eCollisionResponse _response[eCollisionChannel::COLLISION_CHANNEL_END] = {};
	
};
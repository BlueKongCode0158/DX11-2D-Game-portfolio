#include "pch.h"
#include "CollisionProfile.h"

CollisionProfile::CollisionProfile()
{
}

CollisionProfile::~CollisionProfile()
{
}

const eCollisionChannel CollisionProfile::GetChannel() const
{
	return _channel;
}

const std::string CollisionProfile::GetName() const
{
	return _name;
}

const eCollisionResponse CollisionProfile::GetResponse(eCollisionChannel channel)
{
	return _response[channel];
}

void CollisionProfile::Destroy()
{
}

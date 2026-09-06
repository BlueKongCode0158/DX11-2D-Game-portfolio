#include "pch.h"
#include "SoundComponent.h"
#include "../../Object/Asset/AssetManager.h"
SoundComponent::SoundComponent()
{
}

SoundComponent::~SoundComponent()
{
}

bool SoundComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	_type = eComponentType::SOUND;
	return true;
}

void SoundComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
}

void SoundComponent::Destroy()
{
	SceneComponent::Destroy();
}

bool SoundComponent::Save(std::ofstream& file)
{
	return true;
}

bool SoundComponent::Load(std::ifstream& file)
{
	return true;
}

void SoundComponent::SetSound(const std::string& name, bool play)
{
	_sound = SOUND_MANAGER->FindSound(name);
	if (play && _sound)
	{
		_sound->Play();
	}
}

void SoundComponent::SetSound(const std::string& name, const std::string& groupName, const char* fileName, bool loop, bool play)
{
	_sound = SOUND_MANAGER->LoadSound(name, groupName, loop, fileName);
	if (play && _sound)
	{
		_sound->Play();
	}
}

void SoundComponent::Play()
{
	if (_sound)
	{
		_sound->Play();
	}
}

void SoundComponent::Stop()
{
	if (_sound)
	{
		_sound->Stop();
	}
}

void SoundComponent::Pause()
{
	if (_sound)
	{
		_sound->Pause();
	}
}

void SoundComponent::Resume()
{
	if (_sound)
	{
		_sound->Resume();
	}
}

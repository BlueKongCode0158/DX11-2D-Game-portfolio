#include "pch.h"
#include "SoundManager.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::Init()
{
	FMOD_RESULT result = FMOD::System_Create(&_system);
	if (FMOD_OK != result)
	{
		return false;
	}

	result = _system->init(256, FMOD_INIT_NORMAL, nullptr);
	if (FMOD_OK != result)
	{
		return false;
	}

	result = _system->getMasterChannelGroup(&_masterGroup);
	if (FMOD_OK != result)
	{
		return false;
	}
	
	_groups["Master"] = _masterGroup;
	
	CreateChannelGroup("BGM");
	CreateChannelGroup("EFFECT");

	//LoadSound("Default_BGM", "BGM", true, "Sound\\Title.ogg");

	//Play("Default_BGM");
	SetGroupVolume("BGM", 50.f);
	SetGroupVolume("EFFECT", 100.f);
	return true;
}

void SoundManager::Tick()
{
	_system->update();
}

bool SoundManager::CreateChannelGroup(const std::string& name)
{
	FMOD::ChannelGroup* group = FindChannelGroup(name);
	if (group)
	{
		return false;
	}
	
	FMOD_RESULT result = _system->createChannelGroup(name.c_str(), &group);
	if (FMOD_OK != result)
	{
		return false;
	}

	_masterGroup->addGroup(group);
	_groups[name] = group;

	return true;
}

Ptr<Sound> SoundManager::LoadSound(const std::string& name, const std::string& groupName, bool loop, const char* fileName)
{
	FMOD::ChannelGroup* group = FindChannelGroup(groupName);
	if (nullptr == group)
	{
		return nullptr;
	}

	Ptr<Sound> sound = FindSound(name);
	if (nullptr != sound)
	{
		return sound;
	}
	sound = New<Sound>();
	if (!sound->LoadSound(_system, group, loop, fileName))
	{
		DESTROY(sound);
		return nullptr;
	}

	_sounds[name] = sound;
	return _sounds[name];
}

void SoundManager::Play(const std::string& name)
{
	Ptr<Sound> sound = FindSound(name);
	if (nullptr == sound)
	{
		return;
	}

	sound->Play();
}

void SoundManager::Stop(const std::string& name)
{
	Ptr<Sound> sound = FindSound(name);
	if (nullptr == sound)
	{
		return;
	}

	sound->Stop();
}

void SoundManager::Pause(const std::string& name)
{
	Ptr<Sound> sound = FindSound(name);
	if (nullptr == sound)
	{
		return;
	}

	sound->Pause();
}

void SoundManager::Resume(const std::string& name)
{
	Ptr<Sound> sound = FindSound(name);
	if (nullptr == sound)
	{
		return;
	}

	sound->Resume();
}

void SoundManager::SetMasterVolume(float vol)
{
	vol = Utility::Clamp(vol, 0.f, 100.f);
	_masterGroup->setVolume(vol / 100.f);
}

void SoundManager::SetGroupVolume(const std::string& groupName, float vol)
{
	FMOD::ChannelGroup* group = FindChannelGroup(groupName);
	if (nullptr == group)
	{
		return;
	}

	vol = Utility::Clamp(vol, 0.f, 100.f);
	group->setVolume(vol / 100.f);
}

Ptr<Sound> SoundManager::FindSound(const std::string& name)
{
	auto it = _sounds.find(name);
	if (_sounds.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

FMOD::ChannelGroup* SoundManager::FindChannelGroup(const std::string& groupName)
{
	auto it = _groups.find(groupName);
	if (_groups.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

void SoundManager::Destroy()
{
	for (auto& it : _sounds)
	{
		DESTROY(it.second);
	}

	for (auto it : _groups)
	{
		it.second->release();
	}

	if (_system)
	{
		_system->release();
		_system->close();
	}

	_sounds.clear();
	_groups.clear();
	_system = nullptr;
}

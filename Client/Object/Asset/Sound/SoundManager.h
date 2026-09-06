#pragma once
#include "../../SubManager.h"
#include "Sound.h"

class SoundManager : public SubManager
{
public:
	SoundManager();
	virtual ~SoundManager();
private:
	FMOD::System* _system;
	FMOD::ChannelGroup* _masterGroup;
	std::unordered_map<std::string, Ptr<Sound>> _sounds;
	std::unordered_map<std::string, FMOD::ChannelGroup*> _groups;
public:
	bool Init();
	void Tick();
	bool CreateChannelGroup(const std::string& name);
	Ptr<Sound> LoadSound(const std::string& name, const std::string& groupName, bool loop, const char* fileName);
	void Play(const std::string& name);
	void Stop(const std::string& name);
	void Pause(const std::string& name);
	void Resume(const std::string& name);
	void SetMasterVolume(float vol);
	void SetGroupVolume(const std::string& groupName, float vol);
	Ptr<Sound> FindSound(const std::string& name);
	virtual void Destroy() override;
private:
	FMOD::ChannelGroup* FindChannelGroup(const std::string& groupName);

};
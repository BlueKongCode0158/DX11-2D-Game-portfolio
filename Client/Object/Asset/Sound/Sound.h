#pragma once
#include "../Asset.h"
#include "FMOD/fmod.hpp"

class Sound : public Asset
{
public:
	Sound();
	virtual ~Sound();
protected:
	// fmod sound system
	FMOD::System* _system = nullptr;
	// 사운드가 어떤 채널에서 재생되는지 관리하는 객체
	// bgm, effect, ui 등등 사운드 그룹으로 나누어서 관리 가능
	FMOD::ChannelGroup* _group = nullptr;
	FMOD::Channel* _channel = nullptr;
	FMOD::Sound* _sound = nullptr;

	bool _loop = false;
	int32 _playCount = 0;
public:
	bool LoadSound(FMOD::System* system, FMOD::ChannelGroup* group, bool loop, const char* fileName);
	void Play();
	void Stop();
	void Pause();
	void Resume();
	virtual void Destroy() override;
public:
	static FMOD_RESULT SoundCallBack(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType,
		FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2);

};
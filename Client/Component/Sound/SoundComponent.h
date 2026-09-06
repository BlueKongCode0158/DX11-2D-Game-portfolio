#pragma once 
#include "../SceneComponent.h"

class SoundComponent : public SceneComponent
{
public:
	SoundComponent(); 
	virtual ~SoundComponent();
protected:
	Ptr<class Sound> _sound;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime);
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetSound(const std::string& name, bool play);
	void SetSound(const std::string& name, const std::string& groupName, const char* fileName, bool loop, bool play);
	void Play();
	void Stop();
	void Pause();
	void Resume();
};
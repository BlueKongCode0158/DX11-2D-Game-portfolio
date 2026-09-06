#pragma once
#include "../../Object/Object.h"
#include "../../Object/Asset/Animation/Animation2DSequence.h"

class Animation2D : public Object
{
	friend class SpriteComponent;
public :
	Animation2D();
	virtual ~Animation2D();
private:
	Ptr<class AnimCBuffer> _animBuffer;
	Weak<class SpriteComponent> _owner;
	Ptr<class Animation2DSequence> _currentSequences;
	std::unordered_map<std::string, Ptr<class Animation2DSequence>> _sequences;
	bool _flip = false;	// 애니메이션 반전
public:
	void SetAnimFlip(bool flip);
	void SetShader();
public:
	bool AddSequence(const std::string& name, bool loop = false, bool reverse = false, float playTime = 0.f, float playRate = 1.f, float loopFrame = 0.f);
	bool AddSequence(const std::string& sequenceName, const std::string& dataName, int32 startFrame, int32 frameCount, bool loop = false, bool reverse = false, float playTime = 0.f, float playRate = 1.f, float loopFrame = 0.f);
	bool AddSequence(Ptr<class Animation2DData> data, bool loop = false, bool reverse = false, float playTime = 0.f, float playRate = 1.f, float loopFrame = 0.f);
	// loop / playTime(=frame_count/fps) / loopFrame 을 원본 클립에서 baking 된 값으로 자동 설정
	bool AddSequenceAuto(const std::string& name, bool reverse = false, float playRate = 1.f);
	void SetPlayTime(const std::string& name, float playTime);
	void SetPlayRate(const std::string& name, float playRate);
	void SetLoop(const std::string& name, bool loop);
	void SetReverse(const std::string& name, bool reverse);
	void SetPlay(const std::string& name, bool play);
	void SetLoopAnimationFrame(const std::string& name, int32 loopFrame);
	void ChangeAnimation(const std::string& name);
	Ptr<class Animation2DData> GetCurrentSequneceData();
	int32 GetCurrentSequneceDataFrame()
	{
		return _currentSequences->GetFrame();
	}
	void SetPlayCurAnimation(bool play);
public:
	bool GetAnimFlip() const;
public:
	void AddFrameSize(float deltaTime);
public:
	Ptr<Animation2DSequence> FindSequence(const std::string& name);
public:
	Ptr<Animation2DSequence> GetCurSequence();
	bool IsAnimationEnd();
public:
	template<typename T>
	void AddNotify(const std::string& name, int32 frame, T* obj, void(T::* memfunc)())
	{
		Ptr<Animation2DSequence> sq = FindSequence(name);
		if (nullptr == sq)
		{
			return;
		}
		sq->AddNotify(frame, obj, memfunc);
	}

	template<typename T>
	void AddNotify(const std::string& name, int32 frame, T&& func)
	{
		Ptr<Animation2DSequence> sq = FindSequence(name);
		if (nullptr == sq)
		{
			return;
		}
		sq->AddNotify(frame, std::forward<T>(func));
	}
public:
	bool Init();
	void Tick(float deltaTime);
	virtual void Destroy() override;
	virtual void DrawInspector() override;
};

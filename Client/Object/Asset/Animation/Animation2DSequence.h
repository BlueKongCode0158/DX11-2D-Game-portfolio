#pragma once
#include "../../Object.h"
#include <functional>
#include "Animation2DData.h"

struct FAnimation2DNotify
{
	int32 _frame = 0;
	std::vector<std::function<void()>> _funcs;
};

class Animation2DSequence : public Object
{
	friend class Animation2D;
public:
	Animation2DSequence();
	virtual ~Animation2DSequence();
public:
	Ptr<class Animation2DData> GetAnimation2DDate() const;
	int32		GetFrame() const { return _frame; }
	int32		GetFrameCount() const;
	bool		GetPlay() const { return _play; }
	const float GetPlayTime() const { return _playTime; }
	const		std::string& GetName() const;
public:
	void SetName(const std::string& name);
	void SetStartLoopFrame(int32 loopFrame);
	void SetData(Ptr<class Animation2DData> data);
	void SetFrameRange(int32 startFrame, int32 frameCount);
	void SetPlayTime(float playTime);
	void SetPlayRate(float rateTime);
	void SetLoop(bool loop);
	void SetReverse(bool reverse);
	void SetPlay(bool play);
	void SetFrame(int32 frame);
public:
	void AddFrameSize(float deltaTime);
public:
	void Tick(float deltaTime);
	virtual void DrawInspector() override;
	virtual void Destroy() override;
	void InvokeNotify();
public:
	template<typename T>
	void AddNotify(int32 frame, T* obj, void (T::* memFunc)())
	{
		if (frame >= GetFrameCount() || frame < 0)
		{
			return;
		}

		const int32 notifyFrame = _startFrame + frame;
		auto it = _notifyes.find(notifyFrame);
		if (_notifyes.end() != it)
		{
			it->second._funcs.push_back(std::bind(memFunc, obj));
			return;
		}

		FAnimation2DNotify notify;
		notify._frame = notifyFrame;
		notify._funcs.push_back(std::bind(memFunc, obj));
		_notifyes[notifyFrame] = notify;
	}

	template<typename T>
	void AddNotify(int32 frame, T&& func)
	{
		if (frame >= GetFrameCount() || frame < 0)
		{
			return;
		}

		const int32 notifyFrame = _startFrame + frame;
		auto it = _notifyes.find(notifyFrame);
		if (_notifyes.end() != it)
		{
			//			it->second._funcs.push_back(std::forward(func));
			it->second._funcs.push_back(func);
			return;
		}

		FAnimation2DNotify notify;
		notify._frame = notifyFrame;
		//		notify._funcs.push_back(std::forward(func));
		notify._funcs.push_back(func);
		_notifyes[notifyFrame] = notify;

	}
private:
	Weak<class Animation2D> _owner;		// 내가 어떤 애니메이션 클래스에 속해 있는지
	Ptr<class Animation2DData> _data;	// 애니메이션 데이터 저장
	std::string _name;
	int32 _startFrame = 0;
	int32 _endFrame = 0;
	int32 _StartLoopFrame = 0;			// 현재 애니메이션의 루프 프레임이 누구냐?
	int32 _frame = 0;					// 현재 몇번째 프레임
	float _time = 0.f;					// 누적 시간
	float _frameTime = 0;				// 프레임별 시간
	float _playTime = 0;				// 애니메이션 총 플레이 시간
	float _playRate = 0;				// 플레이 속도 배율 
	bool _loop = false;					// 계속 반복할건지
	bool _reverse = false;				// 역으로 재생할건지
	bool _play = true;					// 재생할건지
	std::map<int32, FAnimation2DNotify> _notifyes;
};

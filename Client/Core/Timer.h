#pragma once
#include "Object/Object.h"
#include <functional>

class Timer : public Object
{
public:
	Timer() = default;
	virtual ~Timer() = default;
	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator = (const Timer&) = delete;
	Timer& operator = (Timer&&) = delete;
public:
	const bool IsRun() const;
	void Tick(float deltaTime);
	void Stop();
public:
	virtual void Destroy() override;
private:
	float _expireTime = 0.f;
	float _accTime = 0.f;
	bool _run = false;
	bool _repeat = false;
	std::function<void()> _callback;
public:
	template<typename T>
	void SetTimer(float expireTime, bool repeat, T&& func)
	{
		if (_run)
		{
			return;
		}

		_expireTime = expireTime;
		_repeat = repeat;
		_callback = std::forward<T>(func);
		_accTime = 0.f;
		_run = true;
	}

	template<typename T>
	void SetTimer(float expireTime, bool repeat, T* obj, void(T::* func)())
	{
		if (_run)
		{
			_expireTime = expireTime;
			_repeat = repeat;
			_callback = std::bind(func, obj);
			_accTime = 0.f;
			_run = true;
		}
	}

};
#include "pch.h"
#include "Timer.h"

const bool Timer::IsRun() const
{
	return _run;
}

void Timer::Tick(float deltaTime)
{
	if (!_run)
	{
		return;
	}

	_accTime += deltaTime;
	if (_accTime < _expireTime)
	{
		return;
	}

	_accTime -= _expireTime;
	if (_callback)
		_callback();

	if (!_repeat)
	{
		_run = false;
	}
}

void Timer::Stop()
{
	_run = false;
	_accTime = 0.f;
}

void Timer::Destroy()
{
}

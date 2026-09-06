#include "pch.h"
#include "Animation2DSequence.h"
#include "Animation2DData.h"
#include "../../../Editor/EditEngine.h"


Animation2DSequence::Animation2DSequence()
{
}

Animation2DSequence::~Animation2DSequence()
{
}

Ptr<class Animation2DData> Animation2DSequence::GetAnimation2DDate() const
{
	return _data;
}

int32 Animation2DSequence::GetFrameCount() const
{
	if (!_data)
	{
		return 0;
	}

	return (std::max)(0, _endFrame - _startFrame + 1);
}

const std::string& Animation2DSequence::GetName() const
{
	return _name;
}

void Animation2DSequence::SetName(const std::string& name)
{
	_name = name;
}

void Animation2DSequence::SetStartLoopFrame(int32 loopFrame)
{
	_StartLoopFrame = _startFrame + loopFrame;
}

void Animation2DSequence::SetData(Ptr<class Animation2DData> data)
{
	_data = data;
	if (_data)
	{
		_name = _data->GetName();
		SetFrameRange(0, -1);
	}
}

void Animation2DSequence::SetFrameRange(int32 startFrame, int32 frameCount)
{
	if (!_data)
	{
		return;
	}

	const int32 dataFrameCount = _data->GetFrameCount();
	if (dataFrameCount <= 0)
	{
		_startFrame = 0;
		_endFrame = 0;
		_frame = 0;
		return;
	}

	_startFrame = (std::max)(0, startFrame);
	if (_startFrame >= dataFrameCount)
	{
		_startFrame = dataFrameCount - 1;
	}

	_endFrame = frameCount < 0
		? dataFrameCount - 1
		: (std::min)(dataFrameCount - 1, _startFrame + frameCount - 1);

	if (_endFrame < _startFrame)
	{
		_endFrame = _startFrame;
	}

	_frame = _startFrame;
}

void Animation2DSequence::SetPlayTime(float playTime)
{
	_playTime = playTime;
	if (_data)
	{
		const int32 frameCount = GetFrameCount();
		_frameTime = frameCount > 0 ? playTime / static_cast<float>(frameCount) : 0.f;
	}
}

void Animation2DSequence::SetPlayRate(float rateTime)
{
	_playRate = rateTime;
}

void Animation2DSequence::SetLoop(bool loop)
{
	_loop = loop;
}

void Animation2DSequence::SetReverse(bool reverse)
{
	_reverse = reverse;
}

void Animation2DSequence::SetPlay(bool play)
{
	_play = play;
}

void Animation2DSequence::SetFrame(int32 frame)
{
	if (nullptr == _data)
	{
		return;
	}

	int32 count = GetFrameCount();
	if (count <= 0)
	{
		return;
	}

	if (frame < 0)
	{
		frame = 0;
	}
	else if (frame > count - 1)
	{
		frame = count - 1;
	}

	_frame = _startFrame + frame;
	_time = 0.f;
}

void Animation2DSequence::AddFrameSize(float deltaTime)
{
	if(_data)
	{
		_data->AddFrameSize(deltaTime);
	}
}

void Animation2DSequence::Tick(float deltaTime)
{
	if (!_play)
	{
		return;
	}

	_time += deltaTime * _playRate;

	if (_time >= _frameTime)
	{
		_time -= _frameTime;
		if (_reverse)
		{
			_frame--;
			if (_frame < _startFrame)
			{
				if (_loop)
				{
					_frame = _endFrame;
				}
				else
				{
					_frame = _startFrame;
					SetPlay(false);
				}
			}
		}
		else
		{
			_frame++;
			if (_frame > _endFrame)
			{
				if (_loop)
				{
					_frame = _StartLoopFrame;
				}
				else
				{
					_frame = _endFrame;
					SetPlay(false);
				}
			}
		}

		InvokeNotify();
	}
}

void Animation2DSequence::DrawInspector()
{
	ImGui::BeginGroup();
	ImGui::Checkbox("Play", &_play);
	ImGui::SameLine();
	ImGui::Checkbox("Reverse", &_reverse);
	ImGui::SameLine();
	ImGui::Checkbox("Loop", &_loop);
	ImGui::EndGroup();

	ImGui::DragFloat("PlayTime", &_playTime, 0.1f, 0.f, 100.f);
	ImGui::DragFloat("PlayRate", &_playRate, 0.1f, 0.f, 10.f);

}

void Animation2DSequence::Destroy()
{
	
}

void Animation2DSequence::InvokeNotify()
{
	auto it = _notifyes.find(_frame);
	if (_notifyes.end() == it)
	{
		return;
	}

	FAnimation2DNotify& itVal = it->second;
	size_t size = itVal._funcs.size();

	for (size_t i = 0; i < size; i++)
	{
		if (itVal._funcs[i])
		{
			itVal._funcs[i]();
		}
	}
}

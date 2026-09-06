#include "pch.h"
#include "Animation2DData.h"

#include "../AssetManager.h"
#include "../Texture/Texture.h"
#include "../../../Common/LogManager.h"

Animation2DData::Animation2DData()
{
}

Animation2DData::~Animation2DData()
{
}

eAnimTextureType Animation2DData::GetAnimationTextureType() const
{
	return _type;
}

Ptr<class Texture> Animation2DData::GetTexture() const
{
	return _texture;
}

const FAnimationFrame& Animation2DData::GetFrame(int32 index) const
{
	// 프레임이 없으면 빈 vector 에 인덱싱하게 되므로(특히 size()-1 은 거대한 값) 먼저 막는다.
	if (_frames.empty())
	{
		static const FAnimationFrame empty;
		return empty;
	}

	if (index < 0)
	{
		return _frames[0];
	}
	else if (index >= static_cast<int32>(_frames.size()))
	{
		return _frames[_frames.size() - 1];
	}

	return _frames[index];
}

int32 Animation2DData::GetFrameCount() const
{
	return static_cast<int32>(_frames.size());
}

int32 Animation2DData::AddFrameTexture(Ptr<class Texture> texture)
{
	for (size_t i = 0; i < _frameTextures.size(); ++i)
	{
		if (_frameTextures[i] == texture)
		{
			return static_cast<int32>(i);
		}
	}

	_frameTextures.emplace_back(texture);
	return static_cast<int32>(_frameTextures.size() - 1);
}

void Animation2DData::SetLastFrameTextureIndex(int32 textureIndex)
{
	if (_frames.empty())
	{
		return;
	}
	_frames.back()._textureIndex = textureIndex;
}

Ptr<class Texture> Animation2DData::GetFrameTexture(int32 frameIndex) const
{
	if (_frames.empty())
	{
		return _texture;
	}

	const FAnimationFrame& frame = GetFrame(frameIndex);	// 내부에서 클램프됨
	if (frame._textureIndex >= 0 && frame._textureIndex < static_cast<int32>(_frameTextures.size()))
	{
		return _frameTextures[frame._textureIndex];
	}

	return _texture;
}

void Animation2DData::GetFrameUV(int32 frameIndex, OUT FVector2D& UVLT, OUT FVector2D& UVRB, OUT Ptr<class Texture>& texture)
{
	if (_frames.empty())
	{
		return;
	}
	const FAnimationFrame& frame = GetFrame(frameIndex);
	if (frame._textureIndex >= 0 && frame._textureIndex < static_cast<int32>(_frameTextures.size()))
	{
		
		texture = _frameTextures[frame._textureIndex];
	}

}

void Animation2DData::SetStartLoopFrame(int32 loopFrame)
{
	_LoopStartFrame = loopFrame;
}

float Animation2DData::GetFps() const
{
	return _fps;
}

bool Animation2DData::GetLoop() const
{
	return _loop;
}

int32 Animation2DData::GetStartLoopFrame() const
{
	return _LoopStartFrame;
}

float Animation2DData::GetPlayTime() const
{
	if (_fps <= 0.f)
	{
		return 0.f;
	}
	return static_cast<float>(_frames.size()) / _fps;
}

void Animation2DData::SetFps(float fps)
{
	_fps = fps;
}

void Animation2DData::SetLoop(bool loop)
{
	_loop = loop;
}

void Animation2DData::SetAnimationTextureType(eAnimTextureType type)
{
	_type = type;
}

void Animation2DData::SetTexture(const std::string& name)
{
	_texture = TEXTURE_MANAGER->FindTexture(name);
}

void Animation2DData::SetTexture(Ptr<class Texture> texture)
{
	_texture = texture;
}

void Animation2DData::SetTexture(const std::string& name, const std::wstring& fileName)
{
	if (!TEXTURE_MANAGER->LoadTexture(name, fileName))
	{
		return;
	}
	_texture = TEXTURE_MANAGER->FindTexture(name);
}

void Animation2DData::AddFrame(const FVector2D& start, const FVector2D& size)
{
	AddFrame(start._x, start._y, size._x, size._y);
}

void Animation2DData::AddFrame(const FVector2D& start, const FVector2D& size, const FVector2D& offset)
{
	AddFrame(start._x, start._y, size._x, size._y, offset._x, offset._y);
}

void Animation2DData::AddFrame(const FVector2D& start, const FVector2D& size, const FVector2D& offset, bool flipped, bool rotated)
{
	AddFrame(start._x, start._y, size._x, size._y, offset._x, offset._y, flipped, rotated);
}

void Animation2DData::AddFrame(float startX, float startY, float sizeX, float sizeY)
{
	AddFrame(startX, startY, sizeX, sizeY, 0.f, 0.f);
}

void Animation2DData::AddFrame(float startX, float startY, float sizeX, float sizeY, float offsetX, float offsetY)
{
	AddFrame(startX, startY, sizeX, sizeY, offsetX, offsetY, false, false);
}

void Animation2DData::AddFrame(float startX, float startY, float sizeX, float sizeY, float offsetX, float offsetY, bool flipped, bool rotated)
{
	FAnimationFrame frame;
	frame._start._x = startX;
	frame._start._y = startY;

	frame._size._x = sizeX;
	frame._size._y = sizeY;

	frame._offset._x = offsetX;
	frame._offset._y = offsetY;

	frame._rotated90  = rotated;
	frame._facingFlip = flipped;
	_frames.emplace_back(frame);

}

void Animation2DData::AddFrame(const FVector2D& start, const FVector2D& size, 
	const FVector2D& offset, const FVector2D& draw, const FVector2D& canvas, 
	const FVector2D& pivotTrimmed, const FVector2D& pivotCanvas,
	bool facingFlip, bool rotated)
{
	AddFrame(start._x, start._y, size._x, size._y, draw._x, draw._y, offset._x, offset._y,
		canvas._x, canvas._y, pivotTrimmed._x, pivotTrimmed._y, pivotCanvas._x, pivotCanvas._y,
		facingFlip, rotated);
}

void Animation2DData::AddFrame(float StartX, float StartY, 
	float sizeX, float sizeY, float drawX, float drawY, float offsetX, float offsetY,
	float canvasX, float canvasY, float pivotTrimmedX, float pivotTrimmedY, 
	float pivotCanvasX, float pivotCanvasY, bool facingFlip, bool rotated)
{
	FAnimationFrame frame;
	frame._start._x = StartX;
	frame._start._y = StartY;

	frame._size._x = sizeX;
	frame._size._y = sizeY;

	frame._draw._x = drawX;
	frame._draw._y = drawY;

	frame._offset._x = offsetX;
	frame._offset._y = offsetY;

	frame._canvasSize._x = canvasX;
	frame._canvasSize._y = canvasY;

	frame._pivotTrimmed._x = pivotTrimmedX;
	frame._pivotTrimmed._y = pivotTrimmedY;

	frame._pivotCanvasSize._x = pivotCanvasX;
	frame._pivotCanvasSize._y = pivotCanvasY;

	frame._rotated90 = rotated;
	frame._facingFlip = facingFlip;
	_frames.emplace_back(frame);

}

void Animation2DData::AddFrameSize(float deltaTime)
{
	if (_frames.empty())
	{
		return;
	}

	_frames.front()._size._x += deltaTime;
	_frames.front()._size._y += deltaTime;
}

void Animation2DData::Destroy()
{
}

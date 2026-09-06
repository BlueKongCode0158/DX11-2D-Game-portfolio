#include "pch.h"
#include "Animation.h"
#include "../AssetManager.h"

eAnimTextureType Animation::GetAnimTextureType() const
{
	return eAnimTextureType();
}

Ptr<class Texture> Animation::GetTexture() const
{
	return Ptr<class Texture>();
}

const FAnimationFrame& Animation::GetAnimFrame(int32 index) const
{
	if (index < 0)
	{
		return _frames.front();
	}
	else if (index >= _frames.size())
	{
		return _frames.back();
	}

	return _frames[index];
	// TODO: 여기에 return 문을 삽입합니다.
}

int32 Animation::GetFrameCount() const
{
	return _frames.size();
}

void Animation::SetAnimationTextureType(eAnimTextureType type)
{

}

void Animation::SetTexture(const std::string& name)
{
	_texture = TEXTURE_MANAGER->FindTexture(name);
}

void Animation::SetTexture(Ptr<class Texture> texture)
{
	_texture = texture;
}

void Animation::AddFrame(const FVector2D& start, const FVector2D& size)
{
	AddFrame(start._x, start._y, size._x, size._y);
}

void Animation::AddFrame(float startX, float startY, float sizeX, float sizeY)
{
	FAnimationFrame frame;
	frame._size._x = sizeX;
	frame._size._y = sizeY;

	frame._start._x = startX;
	frame._start._y = startY;

}

#pragma once
#include "../Asset.h"

class Animation : public Asset
{
public:
	Animation();
	virtual ~Animation();
private:
	Ptr<class Texture> _texture;
	eAnimTextureType _type = eAnimTextureType::ANIMATION_TEXTURE_END;
	std::vector<FAnimationFrame> _frames;
public:
	eAnimTextureType GetAnimTextureType() const;
	Ptr<class Texture> GetTexture() const;
	const FAnimationFrame& GetAnimFrame(int32 index) const;
	int32 GetFrameCount() const;

	void SetAnimationTextureType(eAnimTextureType type);
	void SetTexture(const std::string& name);
	void SetTexture(Ptr<class Texture> texture);

	void AddFrame(const FVector2D& start, const FVector2D& size);
	void AddFrame(float startX, float startY, float sizeX, float sizeY);
};
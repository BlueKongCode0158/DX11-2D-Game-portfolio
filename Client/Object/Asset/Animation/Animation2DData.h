#pragma once
#include "../Asset.h"

class Animation2DData : public Asset
{
public:
	Animation2DData();
	virtual ~Animation2DData();
public:
	eAnimTextureType GetAnimationTextureType() const;
	Ptr<class Texture> GetTexture() const;
	const FAnimationFrame& GetFrame(int32 index) const;
	int32 GetFrameCount() const;
	// 프레임별 아틀라스(멀티 텍스처 클립) 지원
	int32 AddFrameTexture(Ptr<class Texture> texture);	// 중복 제거 후 인덱스 반환
	void SetLastFrameTextureIndex(int32 textureIndex);	// 가장 최근에 추가된 프레임에 인덱스 지정
	Ptr<class Texture> GetFrameTexture(int32 frameIndex) const;	// 프레임이 쓰는 텍스처 (없으면 _texture)
	void GetFrameUV(int32 frameIndex, OUT FVector2D& UVLT, OUT FVector2D& UVRB, OUT Ptr<class Texture>& texture);
	// 클립별 재생 정보 (원본 tk2d 클립에서 baking)
	float GetFps() const;
	bool GetLoop() const;
	int32 GetStartLoopFrame() const;
	float GetPlayTime() const;		// frame_count / fps (fps<=0 이면 0)
	void SetFps(float fps);
	void SetLoop(bool loop);

	void SetStartLoopFrame(int32 loopFrame);
	void SetAnimationTextureType(eAnimTextureType type);
	void SetTexture(const std::string& name);
	void SetTexture(Ptr<class Texture> texture);
	void SetTexture(const std::string& name, const std::wstring& fileName);

	void AddFrame(const FVector2D& start, const FVector2D& size);
	void AddFrame(const FVector2D& start, const FVector2D& size, const FVector2D& offset);
	void AddFrame(const FVector2D& start, const FVector2D& size, const FVector2D& offset, bool flipped, bool rotated);

	void AddFrame(float startX, float startY, float sizeX, float sizeY);
	void AddFrame(float startX, float startY, float sizeX, float sizeY, float offsetX, float offsetY);
	void AddFrame(float startX, float startY, float sizeX, float sizeY, float offsetX, float offsetY, bool flipped, bool rotated);
	
	void AddFrame(const FVector2D& start, const FVector2D& size, const FVector2D& offset, 
		const FVector2D& draw, const FVector2D& canvas, const FVector2D& pivotTrimmed, const FVector2D& pivotCanvas, bool facingFlip, bool rotated);
	void AddFrame(float StartX, float StartY,
		float sizeX, float sizeY, float drawX, float drawY, 
		float offsetX, float offsetY,
		float canvasX, float canvasY, float pivotTrimmedX, float pivotTrimmedY,
		float pivotCanvasX, float pivotCanvasY, bool facingFlip, bool rotated
	);
public:
	void AddFrameSize(float deltaTime);
public:
	virtual void Destroy() override;
private:
	Ptr<class Texture> _texture;
	std::vector<Ptr<class Texture>> _frameTextures;	// 클립이 참조하는 distinct 아틀라스들
	eAnimTextureType _type = eAnimTextureType::ANIMATION_TEXTURE_END;
	std::vector<FAnimationFrame> _frames;
	int32 _LoopStartFrame = 0;
	float _fps = 0.f;			// 원본 클립 재생 속도 (frame per second)
	bool _loop = false;			// 원본 wrap_mode 가 Loop / LoopSection 인지
};
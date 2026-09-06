#pragma once
#include "../../SubManager.h"
#include <filesystem>
#include <vector>

class AnimationManager : public SubManager
{
public:
	AnimationManager();
	virtual ~AnimationManager();
private:
	std::unordered_map<std::string, Ptr<class Animation2DData>> _anims;
public:
	bool Init();
	Ptr<class Animation2DData> FindAnimation(const std::string& name);
	const std::unordered_map<std::string, Ptr<class Animation2DData>>& GetAnimations() const { return _anims; }
	bool CreateAnimation(const std::string& name, eAnimTextureType type);
	void SetAnimationTextureType(const std::string& name, eAnimTextureType type);
	void SetTexture(const std::string& name, Ptr<class Texture> texture);
	void SetTexture(const std::string& name, const std::string& textureName);
	void SetTexture(const std::string& name, const std::string& textureName, const std::wstring& fileName);
	void SetStartLoopFrame(const std::string& name ,int32 loopFrame);
	void AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size);
	void AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY);
	void AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& offset);
	void AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY, float offsetX, float offsetY);
	void AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& offset, bool flipped, bool rotated);
	void AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY, float offsetX, float offsetY, bool flipped, bool rotated);
	void AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& draw, const FVector2D& offset, const FVector2D& canvas, const FVector2D& pivotTrimmed, const FVector2D& pivotCanvas, bool facingFlip, bool rotated);
	void AddFrame(const std::string& name, float StartX, float StartY, 
				  float sizeX, float sizeY, float drawX, float drawY, 
				  float offsetX, float offsetY, 
				  float canvasX, float canvasY, float pivotTrimmedX, float pivotTrimmedY, 
				  float pivotCanvasX, float pivotCanvasY, bool facingFlip, bool rotated
	);
private:
	bool LoadAnimation(const std::wstring& fileName);

	// JSON 파싱 / 캐시 로드 양쪽이 만들어내는 공통 중간 표현.
	struct AnimFrameCache
	{
		std::string frameTexKey;
		// AddFrame 시그니처와 동일 순서의 14개 float
		float atlasX = 0.f, atlasY = 0.f, atlasW = 0.f, atlasH = 0.f;
		float drawW = 0.f, drawH = 0.f, offsetX = 0.f, offsetY = 0.f;
		float canvasW = 0.f, canvasH = 0.f, pivotTrimX = 0.f, pivotTrimY = 0.f;
		float pivotCanvasX = 0.f, pivotCanvasY = 0.f;
		bool rotated90 = false;
	};
	struct AnimClipCache
	{
		std::string clipKey;
		std::string textureKey;	// texture_keys[0], 레거시 폴백용
		float fps = 0.f;
		bool loop = false;		// wrap_mode 0 또는 1
		int32 loopStart = 0;
		std::vector<AnimFrameCache> frames;
	};

	// JSON 전체 DOM 파싱 → AnimClipCache 목록. (캐시 미스/무효 시에만 호출)
	bool ParseAnimationJson(const std::filesystem::path& jsonPath, std::vector<AnimClipCache>& out);
	// 중간 표현 → 실제 Animation2DData 생성 (텍스처 로드 포함, 기존 절차와 동일).
	void CreateAnimationsFromClips(const std::vector<AnimClipCache>& clips);

	bool LoadAnimationCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, std::vector<AnimClipCache>& out);
	bool WriteAnimationCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, const std::vector<AnimClipCache>& in);
public:
	virtual void Destroy() override;
};
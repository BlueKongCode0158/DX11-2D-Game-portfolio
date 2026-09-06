#include "pch.h"
#include "AnimationManager.h"
#include "Animation2DData.h"
#include "../AssetManager.h"
#include "../Texture/Texture.h"
#include "../../../Core/DirectoryManager.h"
#include "../../../ThirdParty/nlohmann/json.hpp"
#include "../BinaryIO.h"

namespace
{
	// 캐시 포맷 식별자 / 버전. 포맷이 바뀌면 version 을 올린다.
	constexpr uint32 kAnimCacheMagic = 'A2DA';
	constexpr uint32 kAnimCacheVersion = 1;
}

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
}

bool AnimationManager::Init()
{
	LoadAnimation(L"Tk2dAnimations.json");
	return true;
}

Ptr<class Animation2DData> AnimationManager::FindAnimation(const std::string& name)
{
	auto it = _anims.find(name);
	if (_anims.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

bool AnimationManager::CreateAnimation(const std::string& name, eAnimTextureType type)
{
	auto it = _anims.find(name);
	if (_anims.end() != it)
	{
		return false;
	}

	Ptr<Animation2DData> anim = New<Animation2DData>();
	anim->SetName(name);
	anim->SetAnimationTextureType(type);
	_anims[name] = anim;

	return true;
}

void AnimationManager::SetAnimationTextureType(const std::string& name, eAnimTextureType type)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
	{
		return;
	}
	anim->SetAnimationTextureType(type);
	return;
}

void AnimationManager::SetTexture(const std::string& name, Ptr<class Texture> texture)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
	{
		return;
	}
	anim->SetTexture(texture);
}

void AnimationManager::SetTexture(const std::string& name, const std::string& textureName)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
	{
		return;
	}
	anim->SetTexture(textureName);
}

void AnimationManager::SetTexture(const std::string& name, const std::string& textureName, const std::wstring& fileName)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
	{
		return;
	}
	anim->SetTexture(textureName, fileName);
}

void AnimationManager::SetStartLoopFrame(const std::string& name, int32 loopFrame)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
	{
		return;
	}
	anim->SetStartLoopFrame(loopFrame);
}

void AnimationManager::AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(start, size);
}

void AnimationManager::AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(StartX, StartY, sizeX, sizeY);
}

void AnimationManager::AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& offset)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(start, size, offset);
}

void AnimationManager::AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY, float offsetX, float offsetY)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(StartX, StartY, sizeX, sizeY, offsetX, offsetY);
}

void AnimationManager::AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& offset, bool flipped, bool rotated)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(start, size, offset, flipped, rotated);

}

void AnimationManager::AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY, float offsetX, float offsetY, bool flipped, bool rotated)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(StartX, StartY, sizeX, sizeY, offsetX, offsetY, flipped, rotated);

}

void AnimationManager::AddFrame(const std::string& name, const FVector2D& start, const FVector2D& size, const FVector2D& draw, const FVector2D& offset, const FVector2D& canvas, const FVector2D& pivotTrimmed, const FVector2D& pivotCanvas, bool facingFlip, bool rotated)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(start, size, draw, offset, canvas, pivotTrimmed, pivotCanvas, facingFlip, rotated);
}

void AnimationManager::AddFrame(const std::string& name, float StartX, float StartY, float sizeX, float sizeY, float drawX, float drawY, float offsetX, float offsetY, float canvasX, float canvasY, float pivotTrimmedX, float pivotTrimmedY, float pivotCanvasX, float pivotCanvasY, bool facingFlip, bool rotated)
{
	Ptr<Animation2DData> anim = FindAnimation(name);
	if (!anim)
		return;
	anim->AddFrame(StartX, StartY, sizeX, sizeY, drawX, drawY, offsetX, offsetY, canvasX, canvasY, pivotTrimmedX, pivotTrimmedY, pivotCanvasX, pivotCanvasY, facingFlip, rotated);
}

bool AnimationManager::LoadAnimation(const std::wstring& fileName)
{
	std::filesystem::path filePath;
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");

	if (!resPath.has_value())
	{
		return false;
	}

	if (!DirectoryManager::Instance().GetDirectory(resPath.value(), "Json", filePath))
	{
		return false;
	}

	if (!DirectoryManager::Instance().GetFile(filePath, fileName, filePath))
	{
		return false;
	}

	// 캐시 경로: <json>.cache.bin
	std::filesystem::path cachePath = filePath;
	cachePath.replace_extension(".cache.bin");

	std::vector<AnimClipCache> clips;

	// 1) 유효한 캐시가 있으면 바이너리에서 바로 로드 (JSON 파싱 없음).
	if (LoadAnimationCache(cachePath, filePath, clips))
	{
		CreateAnimationsFromClips(clips);
		return true;
	}

	// 2) 캐시 미스/무효 → JSON 파싱 후 캐시 기록.
	if (!ParseAnimationJson(filePath, clips))
	{
		return false;
	}

	WriteAnimationCache(cachePath, filePath, clips);
	CreateAnimationsFromClips(clips);
	return true;
}

bool AnimationManager::ParseAnimationJson(const std::filesystem::path& jsonPath, std::vector<AnimClipCache>& out)
{
	std::ifstream file(jsonPath, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json root = nlohmann::json::parse(file, nullptr, false);
	if (root.is_discarded() || !root.is_array())
	{
		return false;
	}

	for (const auto& animObj : root)
	{
		if (!animObj.contains("clips") || !animObj["clips"].is_array())
		{
			continue;
		}

		for (const auto& clip : animObj["clips"])
		{
			std::string clipKey = clip.value("game2d_animation_name", "");
			if (clipKey.empty())
			{
				continue;
			}

			if (!clip.contains("texture_keys")	 ||
				!clip["texture_keys"].is_array() ||
				 clip["texture_keys"].empty())
			{
				continue;
			}

			AnimClipCache clipCache;
			clipCache.clipKey = clipKey;
			clipCache.textureKey = clip["texture_keys"][0].get<std::string>();

			// 클립별 재생 정보 baking : fps(속도/타이밍), wrap_mode(루프 여부)
			// wrap_mode : 0=Loop, 1=LoopSection, 2=Once, 3=PingPong, 6=Single
			clipCache.fps = clip.value("fps", 0.f);
			int32 wrapMode = clip.value("wrap_mode", 2);
			clipCache.loop = (wrapMode == 0 || wrapMode == 1);
			clipCache.loopStart = clip.value("loop_start", 0);

			if (clip.contains("frames") && clip["frames"].is_array())
			{
				for (const auto& frameEntry : clip["frames"])
				{
					if (!frameEntry.contains("frame") || !frameEntry["frame"].is_object())
					{
						continue;
					}
					const auto& frame = frameEntry["frame"];

					AnimFrameCache frameCache;
					// 프레임마다 자기 아틀라스를 가질 수 있음 (없으면 클립 기본 텍스처).
					frameCache.frameTexKey = frame.value("texture_key", clipCache.textureKey);
					frameCache.atlasX = frame.value("atlas_x", 0.f);
					frameCache.atlasY = frame.value("atlas_y", 0.f);
					frameCache.atlasW = frame.value("atlas_w", 0.f);
					frameCache.atlasH = frame.value("atlas_h", 0.f);
					frameCache.drawW = frame.value("draw_w", 0.f);
					frameCache.drawH = frame.value("draw_h", 0.f);
					frameCache.offsetX = frame.value("offset_x", 0.f);
					frameCache.offsetY = frame.value("offset_y", 0.f);
					frameCache.canvasW = frame.value("canvas_w", 0.f);
					frameCache.canvasH = frame.value("canvas_h", 0.f);
					frameCache.pivotTrimX = frame.value("pivot_trimmed_x", 0.f);
					frameCache.pivotTrimY = frame.value("pivot_trimmed_y", 0.f);
					frameCache.pivotCanvasX = frame.value("pivot_canvas_x", 0.f);
					frameCache.pivotCanvasY = frame.value("pivot_canvas_y", 0.f);
					frameCache.rotated90 = frame.value("rotated90", false);

					clipCache.frames.push_back(std::move(frameCache));
				}
			}

			out.push_back(std::move(clipCache));
		}
	}
	return true;
}

void AnimationManager::CreateAnimationsFromClips(const std::vector<AnimClipCache>& clips)
{
	for (const AnimClipCache& clipCache : clips)
	{
		const std::string& clipKey = clipCache.clipKey;

		CreateAnimation(clipKey, eAnimTextureType::SPRITE);

		// 클립 기본 텍스쳐 = 첫 번째 아틀라스 (레거시 경로 / 폴백용)
		std::string texturefilePath = "FrameAnim\\" + clipCache.textureKey + ".png";
		std::wstring texturefilePathL;
		texturefilePathL.assign(texturefilePath.begin(), texturefilePath.end());
		SetTexture(clipKey, clipCache.textureKey, texturefilePathL);

		Ptr<Animation2DData> anim = FindAnimation(clipKey);
		if (nullptr == anim)
		{
			continue;
		}

		anim->SetFps(clipCache.fps);
		anim->SetLoop(clipCache.loop);

		for (const AnimFrameCache& frame : clipCache.frames)
		{
			// 프레임마다 자기 아틀라스를 로드하고, 그 인덱스를 프레임에 기록한다.
			std::string framefilePath = "FrameAnim\\" + frame.frameTexKey + ".png";
			std::wstring framefilePathL;
			framefilePathL.assign(framefilePath.begin(), framefilePath.end());

			int32 textureIndex = 0;
			if (TEXTURE_MANAGER->LoadTexture(frame.frameTexKey, framefilePathL))
			{
				textureIndex = anim->AddFrameTexture(TEXTURE_MANAGER->FindTexture(frame.frameTexKey));
			}

			AddFrame(clipKey,
				frame.atlasX, frame.atlasY, frame.atlasW, frame.atlasH,
				frame.drawW, frame.drawH, frame.offsetX, frame.offsetY,
				frame.canvasW, frame.canvasH, frame.pivotTrimX, frame.pivotTrimY,
				frame.pivotCanvasX, frame.pivotCanvasY,
				false, frame.rotated90);

			anim->SetLastFrameTextureIndex(textureIndex);
		}

		if (clipCache.loopStart > 0)
		{
			SetStartLoopFrame(clipKey, clipCache.loopStart);
		}
	}
}

bool AnimationManager::LoadAnimationCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, std::vector<AnimClipCache>& out)
{
	BinaryIO::SourceMeta srcMeta;
	if (!BinaryIO::QuerySourceMeta(jsonPath, srcMeta))
	{
		return false;
	}

	std::ifstream is(cachePath, std::ios::binary);
	if (!is.is_open())
	{
		return false;
	}

	uint32 magic = 0;
	uint32 version = 0;
	BinaryIO::SourceMeta cachedMeta;
	uint32 clipCount = 0;

	if (!BinaryIO::ReadPod(is, magic) || magic != kAnimCacheMagic ||
		!BinaryIO::ReadPod(is, version) || version != kAnimCacheVersion ||
		!BinaryIO::ReadPod(is, cachedMeta.writeTime) ||
		!BinaryIO::ReadPod(is, cachedMeta.fileSize) ||
		!BinaryIO::ReadPod(is, clipCount))
	{
		return false;
	}

	// 원본 JSON 이 바뀌었으면 캐시 무효.
	if (cachedMeta.writeTime != srcMeta.writeTime || cachedMeta.fileSize != srcMeta.fileSize)
	{
		return false;
	}

	out.clear();
	out.reserve(clipCount);

	for (uint32 i = 0; i < clipCount; ++i)
	{
		AnimClipCache clipCache;
		uint8 loopByte = 0;
		uint32 frameCount = 0;

		if (!BinaryIO::ReadStr(is, clipCache.clipKey) ||
			!BinaryIO::ReadStr(is, clipCache.textureKey) ||
			!BinaryIO::ReadPod(is, clipCache.fps) ||
			!BinaryIO::ReadPod(is, loopByte) ||
			!BinaryIO::ReadPod(is, clipCache.loopStart) ||
			!BinaryIO::ReadPod(is, frameCount))
		{
			return false;
		}
		clipCache.loop = (loopByte != 0);

		clipCache.frames.reserve(frameCount);
		for (uint32 f = 0; f < frameCount; ++f)
		{
			AnimFrameCache frame;
			float floats[14] = {};
			uint8 rotatedByte = 0;

			if (!BinaryIO::ReadStr(is, frame.frameTexKey))
			{
				return false;
			}
			is.read(reinterpret_cast<char*>(floats), sizeof(floats));
			if (!BinaryIO::ReadPod(is, rotatedByte) || !is)
			{
				return false;
			}

			frame.atlasX = floats[0];  frame.atlasY = floats[1];  frame.atlasW = floats[2];  frame.atlasH = floats[3];
			frame.drawW = floats[4];   frame.drawH = floats[5];   frame.offsetX = floats[6]; frame.offsetY = floats[7];
			frame.canvasW = floats[8]; frame.canvasH = floats[9]; frame.pivotTrimX = floats[10]; frame.pivotTrimY = floats[11];
			frame.pivotCanvasX = floats[12]; frame.pivotCanvasY = floats[13];
			frame.rotated90 = (rotatedByte != 0);

			clipCache.frames.push_back(std::move(frame));
		}

		out.push_back(std::move(clipCache));
	}

	return true;
}

bool AnimationManager::WriteAnimationCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, const std::vector<AnimClipCache>& in)
{
	BinaryIO::SourceMeta srcMeta;
	if (!BinaryIO::QuerySourceMeta(jsonPath, srcMeta))
	{
		return false;
	}

	std::ofstream os(cachePath, std::ios::binary | std::ios::trunc);
	if (!os.is_open())
	{
		return false;
	}

	BinaryIO::WritePod(os, kAnimCacheMagic);
	BinaryIO::WritePod(os, kAnimCacheVersion);
	BinaryIO::WritePod(os, srcMeta.writeTime);
	BinaryIO::WritePod(os, srcMeta.fileSize);
	BinaryIO::WritePod(os, static_cast<uint32>(in.size()));

	for (const AnimClipCache& clipCache : in)
	{
		BinaryIO::WriteStr(os, clipCache.clipKey);
		BinaryIO::WriteStr(os, clipCache.textureKey);
		BinaryIO::WritePod(os, clipCache.fps);
		BinaryIO::WritePod(os, static_cast<uint8>(clipCache.loop ? 1 : 0));
		BinaryIO::WritePod(os, clipCache.loopStart);
		BinaryIO::WritePod(os, static_cast<uint32>(clipCache.frames.size()));

		for (const AnimFrameCache& frame : clipCache.frames)
		{
			BinaryIO::WriteStr(os, frame.frameTexKey);
			const float floats[14] = {
				frame.atlasX, frame.atlasY, frame.atlasW, frame.atlasH,
				frame.drawW, frame.drawH, frame.offsetX, frame.offsetY,
				frame.canvasW, frame.canvasH, frame.pivotTrimX, frame.pivotTrimY,
				frame.pivotCanvasX, frame.pivotCanvasY
			};
			os.write(reinterpret_cast<const char*>(floats), sizeof(floats));
			BinaryIO::WritePod(os, static_cast<uint8>(frame.rotated90 ? 1 : 0));
		}
	}

	return static_cast<bool>(os);
}

void AnimationManager::Destroy()
{
	for (auto& it : _anims)
	{
		DESTROY(it.second);
	}
	_anims.clear();
}

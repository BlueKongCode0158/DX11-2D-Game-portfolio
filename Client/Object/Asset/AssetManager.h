#pragma once
#include "Singleton.h"
#include "Defines.h"
#include "Mesh/MeshManager.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"
#include "Animation/AnimationManager.h"
#include "Sound/SoundManager.h"
#include "Font/FontManager.h"

class AssetManager : public Singleton<AssetManager>
{
	DECLARE_SINGLETON(AssetManager);
public:
	bool Init();
	virtual void Destroy() override;
public:
	template<typename T>
	Ptr<T> Get(eAssetType type)
	{
		if (_managers.end() == _managers.find(type))
		{
			return nullptr;
		}
		return Cast<SubManager, T>(_managers[type]);
	}

	template<typename T>
	bool RegisterManager(eAssetType type)
	{
		Ptr<T> manager = New<T>();
		if (false == manager->Init())
		{
			Delete(manager);
			return false;
		}
		_managers[type] = manager;
		return true;
	}
private:
	std::map<eAssetType, Ptr<class SubManager>> _managers;
};
#define MESH_MANAGER AssetManager::Instance().Get<MeshManager>(eAssetType::MESH)
#define TEXTURE_MANAGER AssetManager::Instance().Get<TextureManager>(eAssetType::TEXTURE)
#define MATERIAL_MANAGER AssetManager::Instance().Get<MaterialManager>(eAssetType::MATERIAL)
#define ANIMATION_MANAGER AssetManager::Instance().Get<AnimationManager>(eAssetType::ANIMATION)
#define SOUND_MANAGER AssetManager::Instance().Get<SoundManager>(eAssetType::SOUND)
#define FONT_MANAGER AssetManager::Instance().Get<FontManager>(eAssetType::FONT)
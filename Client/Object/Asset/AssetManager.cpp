#include "pch.h"
#include "AssetManager.h"
#include "SubManager.h"
#include "Defines.h"

bool AssetManager::Init()
{
	if (false == RegisterManager<TextureManager>(eAssetType::TEXTURE))
	{
		return false;
	}
	if (false == RegisterManager<MaterialManager>(eAssetType::MATERIAL))
	{
		return false;
	}
	if (false == RegisterManager<MeshManager>(eAssetType::MESH))
	{
		return false;
	}
	if (false == RegisterManager<AnimationManager>(eAssetType::ANIMATION))
	{
		return false;
	}

	if (false == RegisterManager<SoundManager>(eAssetType::SOUND))
	{
		return false;
	}

	if (false == RegisterManager<FontManager>(eAssetType::FONT))
	{
		return false;
	}
	return true;
}


void AssetManager::Destroy()
{
	for (auto& it : _managers)
	{
		it.second->Destroy();
		Delete(it.second);
	}
	_managers.clear();
}

#include "pch.h"
#include "TextureManager.h"
#include "Texture.h"
#include "../../../Common/Info.h"
#include "../../../ThirdParty/nlohmann/json.hpp"

#include <algorithm>

bool TextureManager::Init()
{
	if (!LoadTexture("Team_Cherry", TEXT("team_cherry_logo_main_menu.png")))
	{
		return false;
	}

	if (!LoadTexture("Noise", TEXT("NoiseAndGrain.png")))
	{
		return false;
	}

	if (!LoadTexture(TEXT("MeshAtlasImport.json")))
	{
		return false;
	}

	if (!LoadTexture(TEXT("DirectTextureAtlasImport.json")))
	{
		return false;
	}

	return true;
}

bool TextureManager::LoadTexture(const std::string& name, const std::wstring& fileName)
{
	auto foundTexture = _textures.find(name);
	if (foundTexture != _textures.end())
	{
		const bool loaded = foundTexture->second->LoadTexture(fileName);
		if (loaded)
		{
			auto pendingIt = _pendingTextureFiles.find(name);
			if (pendingIt != _pendingTextureFiles.end())
			{
				std::vector<std::wstring>& pendingFiles = pendingIt->second;
				pendingFiles.erase(
					std::remove(pendingFiles.begin(), pendingFiles.end(), fileName),
					pendingFiles.end());

				if (pendingFiles.empty())
				{
					_pendingTextureFiles.erase(pendingIt);
				}
			}
		}
		return loaded;
	}

	Ptr<Texture> texture = New<Texture>();
	if (!texture->LoadTexture(fileName))
	{
		return false;
	}

	texture->SetName(name);
	_textures[name] = texture;
	auto pendingIt = _pendingTextureFiles.find(name);
	if (pendingIt != _pendingTextureFiles.end())
	{
		std::vector<std::wstring>& pendingFiles = pendingIt->second;
		pendingFiles.erase(
			std::remove(pendingFiles.begin(), pendingFiles.end(), fileName),
			pendingFiles.end());

		if (pendingFiles.empty())
		{
			_pendingTextureFiles.erase(pendingIt);
		}
	}

	return true;
}

bool TextureManager::LoadTexture(const std::wstring& fileName)
{
	std::filesystem::path filePath;
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");

	if (!resPath.has_value())	// 방어코드
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

	std::ifstream file(filePath, std::ios::binary);

	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json root = nlohmann::json::parse(file, nullptr, false);

	if (root.is_discarded())
	{
		return false;
	}

	if (!root.contains("entries"))
	{
		return false;
	}

	const nlohmann::json& TextureInfo = root["entries"];

	if (!TextureInfo.is_array())
	{
		return false;
	}

	for (const auto& TextureJson : TextureInfo)
	{
		std::string spriteName = TextureJson.value("texture_key", "");
		std::string texturePath = TextureJson.value("texture_path", "");

		if (spriteName.empty() || texturePath.empty())
		{
			continue;
		}
		std::wstring wPathName = StringUtility::UTF8ToWstring(texturePath);
		std::vector<std::wstring>& pendingFiles = _pendingTextureFiles[spriteName];

		//int32 startPoint = wPathName.find(L".png");

		//if (std::string::npos != startPoint)
		//{
		//	wPathName.replace(wPathName.begin() + startPoint, wPathName.end(), L".dds");
		//}
		if (std::find(pendingFiles.begin(), pendingFiles.end(), wPathName) == pendingFiles.end())
		{
			pendingFiles.push_back(wPathName);
		}
	}

	return true;
}

Ptr<class Texture> TextureManager::FindTexture(const std::string& name)
{
	auto it = _textures.find(name);
	auto pendingIt = _pendingTextureFiles.find(name);

	if (it != _textures.end() && pendingIt != _pendingTextureFiles.end())
	{
		const std::vector<std::wstring> pendingFiles = pendingIt->second;
		for (const std::wstring& fileName : pendingFiles)
		{
			if (!LoadTexture(name, fileName))
			{
				return nullptr;
			}
		}
	}

	if (_textures.end() == it)
	{
		pendingIt = _pendingTextureFiles.find(name);
		if (pendingIt == _pendingTextureFiles.end())
		{
			return nullptr;
		}

		const std::vector<std::wstring> pendingFiles = pendingIt->second;
		for (const std::wstring& fileName : pendingFiles)
		{
			if (!LoadTexture(name, fileName))
			{
				return nullptr;
			}
		}

		it = _textures.find(name);
		if (_textures.end() == it)
		{
			return nullptr;
		}
	}

	return it->second;
}

void TextureManager::Destroy()
{
	for (auto& it : _textures)
	{
		DESTROY(it.second);
	}
	_textures.clear();
	_pendingTextureFiles.clear();
}

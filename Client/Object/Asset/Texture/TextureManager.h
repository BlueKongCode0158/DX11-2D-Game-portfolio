#pragma once
#include "SubManager.h"

class TextureManager : public SubManager
{
public:
	TextureManager() = default;
	virtual ~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator = (const TextureManager&) = delete;
	TextureManager& operator = (TextureManager&&) = delete;
private:
	std::unordered_map<std::string, Ptr<class Texture>> _textures;

	// 로드할 텍스쳐 
	std::unordered_map<std::string, std::vector<std::wstring>> _pendingTextureFiles;
public:
	virtual bool Init();
	virtual bool LoadTexture(const std::string& name, const std::wstring& fileName);
	virtual bool LoadTexture(const std::wstring& fileName); // json 로드용 
	virtual Ptr<class Texture> FindTexture(const std::string& name);
	const std::unordered_map<std::string, Ptr<class Texture>>& GetTextures() const { return _textures; }
	virtual void Destroy() override;

};

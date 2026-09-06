#pragma once
#include "Asset.h"
#include "DirectXTex.h"
#include "Core/DirectoryManager.h"

//ID3D11ShaderResourceView
//GPU에서 텍스쳐 데이터를 쉐이더가 읽을 수 있도록 하는 뷰

//DirectX
struct FTextureInfo
{
	DirectX::ScratchImage _image;
	ComPtr<ID3D11ShaderResourceView> _srv;
	uint32 _width = 0;
	uint32 _height = 0;
	std::wstring _fileName = L"";
};

class Texture : public Asset
{
public:
	Texture() = default;
	virtual ~Texture() = default;
protected:
	std::vector<FTextureInfo> _textureInfos;
	std::unordered_map<std::wstring, int> _textureFinder;
public:
	const FTextureInfo* GetTexture(int32 idx) const;
	bool LoadTexture(const std::wstring& fileName);
	void SetShader(int32 registerNum, int32 shaderBufferType, int32 textureIndex);
	void ResetShader(int32 registerNum, int32 shaderBufferType);
public:
	uint32 GetWidth(int32 idx = 0);
	uint32 GetHeight(int32 idx = 0);
public:
	virtual void Destroy() override;
protected:
	bool CreateResourceView(int32 idx);
};
#include "pch.h"
#include "Texture.h"
#include "Core/Device.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#endif // _DEBUG
#pragma comment(lib, "DirectXTex.lib")

// 텍스쳐 정보 가져오기 
const FTextureInfo* Texture::GetTexture(int32 idx) const
{
	if (idx < 0 || static_cast<size_t>(idx) >= _textureInfos.size())
	{
		return nullptr;
	}
	return &_textureInfos[idx];
}

// 텍스쳐 불러오기 
bool Texture::LoadTexture(const std::wstring& fileName)
{
	// 중복 텍스쳐는 안 들어가게 막았는데 해당 문구 삭제하면
	// 중복 텍스쳐도 들어갈 수 있음.
	if (_textureFinder.find(fileName) != _textureFinder.end())
	{
		return true;
	}

	std::filesystem::path filePath;
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");
	if (!resPath.has_value())
	{
		return false;
	}

	// 리소스 폴더 아래 텍스쳐 폴더가 있는지
	// 있다면 그 경로를 filePath에 넣어준다.
	if (!DirectoryManager::Instance().GetDirectory(resPath.value(), "Texture", filePath))
	{
		return false;
	}

	// .dds 우선: 요청 경로(예: Resources\Texture\FrameAnim\x.png) 옆에 형제 .dds 가 있으면
	// 그것을 로드한다(BC 압축 → 디코드 비용 거의 없음, VRAM 절감). 없으면 기존 .png 로 폴백.
	// 원본 .png 는 그대로 둔다.
	std::filesystem::path requested = filePath / fileName;
	std::filesystem::path ddsPath = requested;
	ddsPath.replace_extension(L".dds");

	if (DirectoryManager::Instance().IsFile(ddsPath))
	{
		filePath = ddsPath;
	}
	else if (!DirectoryManager::Instance().GetFile(filePath, fileName, filePath))
	{
		return false;
	}

	FTextureInfo texture;

	// 파일의 확장자에 따라 로드하는 함수가 달라지므로 
	if (DirectoryManager::Instance().IsExtension(filePath, ".DDS"))
	{
		if (FAILED(DirectX::LoadFromDDSFile(filePath.wstring().c_str(), DirectX::DDS_FLAGS_NONE, nullptr, texture._image)))
		{
			return false;
		}
	}
	else if (DirectoryManager::Instance().IsExtension(filePath, ".TGA"))
	{
		if (FAILED(DirectX::LoadFromTGAFile(filePath.wstring().c_str(), nullptr, texture._image)))
		{
			return false;
		}
	}
	else
	{
		if (FAILED(DirectX::LoadFromWICFile(filePath.wstring().c_str(), DirectX::WIC_FLAGS_NONE ,nullptr, texture._image)))
		{
			return false;
		}
	}

	size_t srcIndex = _textureInfos.size();
	texture._fileName = fileName;
	texture._width = texture._image.GetImages()->width;
	texture._height = texture._image.GetImages()->height;
	_textureFinder.insert(std::make_pair(fileName, static_cast<int>(srcIndex)));
	_textureInfos.emplace_back(std::move(texture));

	return CreateResourceView(srcIndex);
}

void Texture::SetShader(int32 registerNum, int32 shaderBufferType, int32 textureIndex)
{
	if (shaderBufferType & SHADER_TYPE::VERTEX)
	{
		Device::Instance().GetContext()->VSSetShaderResources(registerNum, 1, _textureInfos[textureIndex]._srv.GetAddressOf());
	}

	if (shaderBufferType & SHADER_TYPE::PIXEL)
	{
		Device::Instance().GetContext()->PSSetShaderResources(registerNum, 1, _textureInfos[textureIndex]._srv.GetAddressOf());
	}
}


void Texture::ResetShader(int32 registerNum, int32 shaderBufferType)
{
	// 렌더링이 끝나면 쉐이더 리소스를 해제해야한다.
	ID3D11ShaderResourceView* src = nullptr;

	if (shaderBufferType & SHADER_TYPE::VERTEX)
	{
		Device::Instance().GetContext()->VSSetShaderResources(registerNum, 1, &src);
	}

	if (shaderBufferType & SHADER_TYPE::PIXEL)
	{
		Device::Instance().GetContext()->PSSetShaderResources(registerNum, 1, &src);
	}

}

uint32 Texture::GetWidth(int32 idx)
{
	return _textureInfos[idx]._width;
}

uint32 Texture::GetHeight(int32 idx)
{
	return _textureInfos[idx]._height;
}

void Texture::Destroy()
{
}

bool Texture::CreateResourceView(int32 idx)
{
	if (FAILED(DirectX::CreateShaderResourceView(DEVICE.Get(), _textureInfos[idx]._image.GetImages(), _textureInfos[idx]._image.GetImageCount(), _textureInfos[idx]._image.GetMetadata(), _textureInfos[idx]._srv.GetAddressOf())))
	{
		return false;
	}
	return true;
}

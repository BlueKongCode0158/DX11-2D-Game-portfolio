#include "pch.h"
#include "FontCollection.h"

#include "../../../Core/DirectoryManager.h"

FontCollection::FontCollection()
{
}

FontCollection::~FontCollection()
{
}

bool FontCollection::LoadCollection(IDWriteFactory5* factory, const std::wstring& fileName)
{
	std::filesystem::path filePath;
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");

	if (!DirectoryManager::Instance().GetDirectory(resPath.value(), "Font", filePath))
	{
		return false;
	}

	if (!DirectoryManager::Instance().GetFile(filePath, fileName, filePath))
	{
		return false;
	}

	ComPtr<IDWriteFontFile> fontFile = nullptr;

	if (FAILED(factory->CreateFontFileReference(filePath.c_str(), nullptr, fontFile.GetAddressOf())))
	{
		return false;
	}

	ComPtr<IDWriteFontSetBuilder1> fontBuilder = nullptr;
	if (FAILED(factory->CreateFontSetBuilder(fontBuilder.GetAddressOf())))
	{
		return false;
	}

	fontBuilder->AddFontFile(fontFile.Get());

	ComPtr<IDWriteFontSet> fontSet = nullptr;

	if (FAILED(fontBuilder->CreateFontSet(fontSet.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(factory->CreateFontCollectionFromFontSet(fontSet.Get(), _collection.GetAddressOf())))
	{
		return false;
	}

	UINT32 count = _collection->GetFontFamilyCount();
	ComPtr<IDWriteFontFamily> family = nullptr;

	if (FAILED(_collection->GetFontFamily(0, family.GetAddressOf())))
	{
		return false;
	}

	ComPtr<IDWriteLocalizedStrings> localName = nullptr;
	if (FAILED(family->GetFamilyNames(localName.GetAddressOf())))
	{
		return false;
	}

	if (FAILED(localName->GetString(0, _fontFaceName, 128)))
	{
		return false;
	}
	return true;
}

ComPtr<IDWriteFontCollection> FontCollection::GetCollection()
{
	return _collection;
}

void FontCollection::Destroy()
{
}

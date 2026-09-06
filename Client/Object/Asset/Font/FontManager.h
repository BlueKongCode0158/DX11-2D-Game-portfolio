#pragma once
#include "../../SubManager.h"

class FontManager : public SubManager
{
public:
	FontManager();
	virtual ~FontManager();
private:
	ComPtr<IDWriteFactory5> _writeFactory;
	std::unordered_map<std::string, Ptr<class Font>> _fonts;
	std::unordered_map<std::string, Ptr<class FontCollection>> _fontCollections;
	std::map<uint32, ComPtr<ID2D1SolidColorBrush>> _fontColors;
public:
	bool Init();
	bool LoadFont(const std::string& name, const std::string& collectionName, const TCHAR* fontName, int32 weight, float fontSize,
		const TCHAR* localName, int32 stretch);
	bool LoadFontCollection(const std::string& name, const std::wstring& fileName);
	const TCHAR* GetFontFaceName(const std::string& collectionName);
	bool CreateFontColor(uint8 r, uint8 g, uint8 b, uint8 a);
	bool CreateFontColor(const FVector4D& color);
	Ptr<class Font> FindFont(const std::string& name);
	Ptr<class FontCollection> FindFontCollection(const std::string& name);

	ID2D1SolidColorBrush* FindFontColor(uint8 r, uint8 g, uint8 b, uint8 a);
	ID2D1SolidColorBrush* FindFontColor(const FVector4D& color);
public:
	void Destroy();
};
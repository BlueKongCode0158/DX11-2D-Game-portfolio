#pragma once
#include "../Asset.h"

class Font : public Asset
{
	friend class FontManager;
public:
	Font();
	virtual ~Font();
private:
	ComPtr<IDWriteFactory5> _factory;
	ComPtr<IDWriteTextFormat> _format;
public:
	bool LoadFond(IDWriteFactory5* factory, IDWriteFontCollection* collection, const std::wstring fontName, int32 weight, float fontSize,
		const std::wstring& localName, int32 stretch);
	IDWriteTextLayout* CreateLayout(const std::wstring& text, int32 length, float width, float height);
	virtual void Destroy() override;
};
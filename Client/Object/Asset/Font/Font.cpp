#include "pch.h"
#include "Font.h"

Font::Font()
{
}

Font::~Font()
{
}

bool Font::LoadFond(IDWriteFactory5* factory, IDWriteFontCollection* collection, const std::wstring fontName, int32 weight, float fontSize, const std::wstring& localName, int32 stretch)
{
	_factory = factory;

	if(FAILED(_factory->CreateTextFormat(fontName.c_str(), collection, (DWRITE_FONT_WEIGHT)weight, DWRITE_FONT_STYLE_NORMAL, (DWRITE_FONT_STRETCH)stretch, fontSize, localName.c_str(), _format.GetAddressOf())))
	{
		return false;
	}

	return true;
}

IDWriteTextLayout* Font::CreateLayout(const std::wstring& text, int32 length, float width, float height)
{
	IDWriteTextLayout* layout = nullptr;

	if (FAILED(_factory->CreateTextLayout(text.c_str(), length, _format.Get(), width, height, &layout)))
	{
		return nullptr;
	}
	return layout;
}

void Font::Destroy()
{
}

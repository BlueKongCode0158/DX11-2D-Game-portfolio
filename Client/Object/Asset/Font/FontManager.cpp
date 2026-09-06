#include "pch.h"
#include "FontManager.h"
#include "Font.h"
#include "FontCollection.h"
#include "../../../Core/Device.h"

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

bool FontManager::Init()
{
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(_writeFactory), (IUnknown**)_writeFactory.GetAddressOf())))
    {
        return false;
    }

    LoadFontCollection("NotoSerifCJKsc-Regular", TEXT("NotoSerifCJKsc-Regular.otf"));
    LoadFont("NotoSerifCJKsc-Regular", "NotoSerifCJKsc-Regular", GetFontFaceName("NotoSerifCJKsc-Regular"), 600, 20, TEXT("ko"), DWRITE_FONT_STRETCH_NORMAL);

    CreateFontColor(0, 0, 0, 255);
    CreateFontColor(255, 255, 255, 255);
    CreateFontColor(255, 0, 0, 255);
    CreateFontColor(0, 255, 0, 255);
    CreateFontColor(0, 0, 255, 255);

    return true;
}

bool FontManager::LoadFont(const std::string& name, const std::string& collectionName, const TCHAR* fontName, int32 weight, float fontSize, const TCHAR* localName, int32 stretch)
{
    Ptr<Font> font = FindFont(name);
    if (font)
    {
        return true;
    }

    Ptr<FontCollection> foundCollection = FindFontCollection(collectionName);
    if (!foundCollection)
    {
        return false;
    }

    font = New<Font>();
    font->_name = name;

    if (!font->LoadFond(_writeFactory.Get(), foundCollection->GetCollection().Get(), fontName, weight, fontSize, localName, stretch))
    {
        return false;
    }
    _fonts[name] = font;
    return true;
}

bool FontManager::LoadFontCollection(const std::string& name, const std::wstring& fileName)
{
    Ptr<FontCollection> fontCollection = FindFontCollection(name);
    if (fontCollection)
    {
        return true;
    }

    fontCollection = New<FontCollection>();
    fontCollection->_name = name;

    if (!fontCollection->LoadCollection(_writeFactory.Get(), fileName))
    {
        DESTROY(fontCollection);
        return false;
    }

    _fontCollections[name] = fontCollection;

    return true;
}

const TCHAR* FontManager::GetFontFaceName(const std::string& collectionName)
{
    Ptr<FontCollection> col = FindFontCollection(collectionName);
    if (!col)
    {
        return nullptr;
    }
    return col->GetFondFaceName();
}

bool FontManager::CreateFontColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    ID2D1SolidColorBrush* brush = FindFontColor(r, g, b, a);
    if (brush)
    {
        return true;
    }

    uint32 color = a;
    color = color << 8;

    color = color | r;
    color = color << 8;

    color = color | g;
    color = color << 8;

    color = color | b;

    ComPtr<ID2D1SolidColorBrush> newBrush;
    if (FAILED(Device::Instance().GetTarget2D()->CreateSolidColorBrush(D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f), newBrush.GetAddressOf())))
    {
        return false;
    }

    _fontColors[color] = newBrush;
    return true;
}

bool FontManager::CreateFontColor(const FVector4D& color)
{
    ID2D1SolidColorBrush* brush = FindFontColor(color);
    if (brush)
    {
        return true;
    }

    unsigned char r, g, b, a;
    r = (byte)(color._x * 255);
    g = (byte)(color._y * 255);
    b = (byte)(color._z * 255);
    a = (byte)(color._w * 255);

    uint32 key = a;
    key = key << 8;

    key = key | r;
    key = key << 8;

    key = key | g;
    key = key << 8;

    key = key | b;

    ComPtr<ID2D1SolidColorBrush> newBrush;
    if (FAILED(Device::Instance().GetTarget2D()->CreateSolidColorBrush(D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f), newBrush.GetAddressOf())))
    {
        return false;
    }

    _fontColors[key] = newBrush;
    return true;
}

Ptr<class Font> FontManager::FindFont(const std::string& name)
{
    auto it = _fonts.find(name);
    if (_fonts.end() == it)
    {
        return nullptr;
    }

    return it->second;
}

Ptr<class FontCollection> FontManager::FindFontCollection(const std::string& name)
{
    auto it = _fontCollections.find(name);
    if (_fontCollections.end() == it)
    {
        return nullptr;
    }
    return it->second;
}

ID2D1SolidColorBrush* FontManager::FindFontColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    uint32 color = a;
    color = color << 8;

    color = color | r;
    color = color << 8;

    color = color | g;
    color = color << 8;

    color = color | b;

    auto it = _fontColors.find(color);
    if (_fontColors.end() == it)
    {
        return nullptr;
    }
    return it->second.Get();
}

ID2D1SolidColorBrush* FontManager::FindFontColor(const FVector4D& color)
{
    unsigned char r, g, b, a;
    r = (byte)(color._x * 255);
    g = (byte)(color._y * 255);
    b = (byte)(color._z * 255);
    a = (byte)(color._w * 255);

    uint32 key = a;
    key = key << 8;

    key = key | r;
    key = key << 8;

    key = key | g;
    key = key << 8;

    key = key | b;

    auto it = _fontColors.find(key);
    if (_fontColors.end() == it)
    {
        return nullptr;
    }

    return (it->second).Get();
}

void FontManager::Destroy()
{
    for (auto& it : _fonts)
    {
        DESTROY(it.second);
    }

    for (auto& it : _fontCollections)
    {
        DESTROY(it.second);
    }

    _fonts.clear();
    _fontCollections.clear();
    
}

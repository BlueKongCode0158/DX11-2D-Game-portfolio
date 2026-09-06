#pragma once
#include "../../Asset/Asset.h"

class FontCollection : public Asset
{
	friend class FontManager;
public:
	FontCollection();
	virtual ~FontCollection();
private:
	ComPtr<IDWriteFontCollection1> _collection;
	TCHAR _fontFaceName[128] = {};
public:
	const TCHAR* GetFondFaceName()
	{
		return _fontFaceName;
	}
public:
	bool LoadCollection(IDWriteFactory5* factory, const std::wstring& fileName);
	ComPtr<IDWriteFontCollection> GetCollection();
public:
	virtual void Destroy() override;
};
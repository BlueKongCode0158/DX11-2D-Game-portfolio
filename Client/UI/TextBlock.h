#pragma once
#include "Widget.h"
#include "../Object/Asset/Font/Font.h"

enum class eTextAlignH
{
	LEFT,
	CENTER,
	RIGHT,
	END
};

enum class eTextAlignV
{
	TOP,
	MIDDLE,
	BOTTOM,
	END
};

class TextBlock : public Widget
{
public:
	TextBlock();
	virtual ~TextBlock();
protected:
	ComPtr<ID2D1RenderTarget> _renderTarget;
	ComPtr<IDWriteTextLayout> _textLayout;
	ComPtr<ID2D1SolidColorBrush> _textColor;

	std::wstring _text;
	Ptr<Font> _font;

	bool _bTransparency = false;
	float _opacity = 1.f;

	float _fontSize = 20.f;
	eTextAlignH _alignH = eTextAlignH::CENTER;
	eTextAlignV _alignV = eTextAlignV::MIDDLE;
private:
	bool _layoutDirty = true;
	std::wstring _prvText;
public:
	const TCHAR* GetText() const { return _text.c_str(); }
	int32 GetTextCount() const { return static_cast<int32>(_text.length()); }

	void SetFont(const std::string& name);
	void SetText(const std::wstring& text);
	void SetTextFromInt(int32 number);
	void SetTextFromFloat(float number);
	void AddText(const std::wstring& text);
	void AddTextFromInt(int32 number);
	void AddTextFromFloat(float number);
	void PopBack();
	void ClearText();

	void SetTransparency(bool transparency);
	void SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a);
	void SetTextColor(const FVector4D& color);
	void SetOpacity(float op);

	void SetFontSize(float size);
	void SetAlignH(eTextAlignH align);
	void SetAlignW(eTextAlignV align);
private:
	void CreateTextLayout();
public:
	virtual bool Init(int32 id, const std::string& name, Weak<class Level> level);
	virtual void Tick(float deltaTime);
	virtual void Render();
	virtual void Render(const FVector3D& pos); // 액터에게 붙어야할 때
	virtual void SetSize(const FVector2D& size) override;
};
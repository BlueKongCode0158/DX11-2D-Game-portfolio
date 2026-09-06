#include "pch.h"
#include "TextBlock.h"
#include "../Core/Device.h"
#include "../Object/Asset/AssetManager.h"
#include "../World/Level.h"

TextBlock::TextBlock()
{
}

TextBlock::~TextBlock()
{
}

void TextBlock::CreateTextLayout()
{
	_textLayout.Reset();
	_textLayout = _font->CreateLayout(_text, static_cast<int32>(_text.length()), _size._x, _size._y);
	
	if (!_textLayout)
	{
		return;
	}

	DWRITE_TEXT_RANGE range = {};
	range.startPosition = 0;
	range.length = (UINT32)_text.length();

	_textLayout->SetFontSize(_fontSize, range);

	switch (_alignV)
	{
	case eTextAlignV::TOP:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case eTextAlignV::MIDDLE:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case eTextAlignV::BOTTOM:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	case eTextAlignV::END:
		break;
	default:
		break;
	}

	switch (_alignH)
	{
	case eTextAlignH::LEFT:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case eTextAlignH::CENTER:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case eTextAlignH::RIGHT:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	case eTextAlignH::END:
		break;
	default:
		break;
	}
}

bool TextBlock::Init(int32 id, const std::string& name, Weak<class Level> level)
{
	Widget::Init(id, name, level);

	_renderTarget = Device::Instance().GetTarget2D();

	SetFont("NotoSerifCJKsc-Regular");
	SetTextColor(0, 0, 0, 255);

	CreateTextLayout();
	return true;
}

void TextBlock::Tick(float deltaTime)
{
	Widget::Tick(deltaTime);
}

void TextBlock::Render()
{
	_renderTarget->BeginDraw();
	if (!IsEnable())
	{
		return;
	}

	if (_layoutDirty)
	{
		CreateTextLayout();
		_layoutDirty = false;
	}

	FResolution rs = Device::Instance().GetRS();

	D2D1_POINT_2F point;
	point.x = _renderPos._x;
	point.y = rs._height - _renderPos._y - _size._y;

	if (_bTransparency)
	{
		_textColor->SetOpacity(_opacity);
	}
	else
	{
		_textColor->SetOpacity(1.f);
	}

	_renderTarget->DrawTextLayout(point, _textLayout.Get(), _textColor.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
	Widget::Render();
	_renderTarget->EndDraw();
}

void TextBlock::Render(const FVector3D& pos)
{
	_renderTarget->BeginDraw();
	if (!IsEnable())
	{
		return;
	}
	FResolution rs = Device::Instance().GetRS();
	
	auto level = Lock<Level>(_level);
	if (level)
	{
		return;
	}

	const FVector3D& camPos = level->GetCameraWorldPos();
	
	FVector2D renderPos = _renderPos;
	renderPos._x += pos._x;
	renderPos._y += pos._y;

	renderPos._x -= camPos._x;
	renderPos._y -= camPos._y;

	renderPos._x += (rs._width * 0.5f);
	renderPos._y += (rs._height * 0.5f);

	D2D1_POINT_2F point;
	point.x = renderPos._x;
	point.y = rs._height - renderPos._y - _size._y;

	if (_bTransparency)
	{
		_textColor->SetOpacity(_opacity);
	}
	else
	{
		_textColor->SetOpacity(1.f);
	}
	_renderTarget->DrawTextLayout(point, _textLayout.Get(), _textColor.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
	Widget::Render(pos);
	_renderTarget->EndDraw();
}

void TextBlock::SetSize(const FVector2D& size)
{
	if (_size == size)
	{
		return;
	}

	Widget::SetSize(size);
	_layoutDirty = true;
}


void TextBlock::SetFont(const std::string& name)
{
	if (name.empty())
	{
		return;
	}
	_font = FONT_MANAGER->FindFont(name);
	_layoutDirty = true;
}

void TextBlock::SetText(const std::wstring& text)
{
	if (_text == text)
	{
		return;
	}
	_text = text;
	_layoutDirty = true;
}

void TextBlock::SetTextFromInt(int32 number)
{
	_text = std::to_wstring(number);
	_layoutDirty = true;
}

void TextBlock::SetTextFromFloat(float number)
{
	_text = std::to_wstring(number);
	_layoutDirty = true;
}

void TextBlock::AddText(const std::wstring& text)
{
	_text += text;
	_layoutDirty = true;
}

void TextBlock::AddTextFromInt(int32 number)
{
	_text += std::to_wstring(number);
	_layoutDirty = true;
}

void TextBlock::AddTextFromFloat(float number)
{
	_text += std::to_wstring(number);
	_layoutDirty = true;
}

void TextBlock::PopBack()
{
	_text.pop_back();
	_layoutDirty = true;

}

void TextBlock::ClearText()
{
	_text.clear();
	_layoutDirty = true;
}

void TextBlock::SetTransparency(bool transparency)
{
	_bTransparency = transparency;
}

void TextBlock::SetTextColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	_textColor = FONT_MANAGER->FindFontColor(r, g, b, a);
}

void TextBlock::SetTextColor(const FVector4D& color)
{
	_textColor = FONT_MANAGER->FindFontColor(color);
}

void TextBlock::SetOpacity(float op)
{
	_opacity = op;
}

void TextBlock::SetFontSize(float size)
{
	_fontSize = size;
	if (!_textLayout)
	{
		return;
	}

	DWRITE_TEXT_RANGE range = {};
	range.startPosition = 0;
	range.length = static_cast<UINT32>(_text.length());

	_textLayout->SetFontSize(_fontSize, range);
}

void TextBlock::SetAlignH(eTextAlignH align)
{
	_alignH = align;

	if (!_textLayout)
	{
		return;
	}

	switch (_alignH)
	{
	case eTextAlignH::LEFT:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case eTextAlignH::CENTER:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case eTextAlignH::RIGHT:
		_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	case eTextAlignH::END:
		break;
	default:
		break;
	}

}

void TextBlock::SetAlignW(eTextAlignV align)
{
	_alignV = align;

	if (!_textLayout)
	{
		return;
	}

	switch (_alignV)
	{
	case eTextAlignV::TOP:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case eTextAlignV::MIDDLE:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case eTextAlignV::BOTTOM:
		_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	case eTextAlignV::END:
		break;
	default:
		break;
	}

}

#include "pch.h"
#include "Button.h"
#include "../Object/Asset/AssetManager.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Object/Asset/Texture/Texture.h"
#include "../Object/Asset/Animation/Animation2DSequence.h"
#include "../Core/Device.h"

#include "../Shader/ConstantBuffer/UICBuffer.h"

#include "../Shader/ShaderManager.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/UIShader.h"

#include "../World/Level.h"


Button::Button()
{
}

Button::~Button()
{
}


void Button::SetTexture(UI_BUTTON_STATE::Type type, const std::string& name, const std::wstring& fileName)
{
	if (!TEXTURE_MANAGER->LoadTexture(name, fileName))
	{
		return;
	}
	SetTexture(type, name);
}

void Button::SetTexture(UI_BUTTON_STATE::Type type, const std::string& name)
{
	SetTexture(type, TEXTURE_MANAGER->FindTexture(name));
}

void Button::SetTexture(UI_BUTTON_STATE::Type type, Ptr<class Texture> texture)
{
	_brush[type]._texture = texture;
}

void Button::SetOpacity(UI_BUTTON_STATE::Type type, float op)
{
	_brush[type]._tint._w = op;

}

void Button::SetTint(UI_BUTTON_STATE::Type type, float r, float g, float b, float a)
{
	_brush[type]._tint._x = r;
	_brush[type]._tint._y = g;
	_brush[type]._tint._z = b;
	_brush[type]._tint._w = a;

}

void Button::SetBrushAnimEnable(UI_BUTTON_STATE::Type type, bool enable)
{
	_brush[type]._animEnable = enable;
}

void Button::AddBrushFrame(UI_BUTTON_STATE::Type type, const FVector2D& start, const FVector2D& size)
{
	AddBrushFrame(type, start._x, start._y, size._x, size._y);
}

void Button::AddBrushFrame(UI_BUTTON_STATE::Type type, float startX, float startY, float sizeX, float sizeY)
{
	FAnimationFrame frame;
	frame._start._x = startX;
	frame._start._y = startY;

	frame._size._x = sizeX;
	frame._size._y = sizeY;

	_brush[type]._frames.push_back(frame);
	//_brush[type]._frameTime = _brush[type]._playTime / _brush[type]._frames.size();
}

void Button::SetCurrentFrame(UI_BUTTON_STATE::Type type, int32 frame)
{
	_brush[type]._sequence->SetFrame(frame);
}

void Button::SetAnimationPlayTime(UI_BUTTON_STATE::Type type, float time)
{
	_brush[type]._sequence->SetPlayTime(time);
}

void Button::SetAnimationPlayRate(UI_BUTTON_STATE::Type type, float rate)
{
	_brush[type]._playRate = rate;
}

bool Button::Init(int32 id, const std::string& name, Weak<class Level> level)
{
	Widget::Init(id, name, level);
	_buttonState = UI_BUTTON_STATE::ENABLE;

	return true;
}

void Button::Tick(float deltaTime)
{
	Widget::Tick(deltaTime);
}

void Button::Render()
{
	if (!IsEnable() || !IsActive())
	{
		return;
	}

	auto level = Lock<Level>(_level);
	if (!level)
	{
		return;
	}

	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
	FMatrix matScale, matRot, matTrans, matWorld;

	matWorld = matScale * matRot * matTrans;
	
	FMatrix matView;

	_transformCBuffer->SetWorldMatrix(matWorld);
	_transformCBuffer->SetViewMatrix(matView);
	_transformCBuffer->SetProjMatrix(level->GetUIProjMatrix());
	_transformCBuffer->Update();

	_uiCBuffer->SetTint(_brush[_buttonState]._tint);

	if (_brush[_buttonState]._texture)
	{
		_uiCBuffer->SetTextureEnable(true);
		_brush[_buttonState]._texture->SetShader(0, SHADER_TYPE::PIXEL, 0);
	}
	else
	{
		_uiCBuffer->SetTextureEnable(false);
	}

	if (_brush[_buttonState]._animEnable)
	{
		_brush[_buttonState].SetShader(_uiCBuffer);
	}
	else
	{
		_uiCBuffer->SetAnimEnable(false);
	}

	_uiCBuffer->Update();
	_shader->SetShader();
	_mesh->Render();

	Widget::Render();

}	

void Button::Render(const FVector3D& pos)
{
	if (!IsEnable() || !IsActive())
	{
		return;
	}
	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
	
	auto level = Lock<Level>(_level);
	if (!level)
	{
		return;
	}

	FResolution rs = Device::Instance().GetRS();
	const FVector3D& camPos = level->GetCameraWorldPos();

	FVector2D renderPos = _renderPos;
	renderPos._x += pos._x;
	renderPos._y += pos._y;

	renderPos._x -= camPos._x;
	renderPos._y -= camPos._y;

	FMatrix matScale, matRot, matTrans, matWorld;
	matScale.Scaling(_size);
	matTrans.Translation(renderPos);

	matWorld = matScale * matRot * matTrans;

	FMatrix matView;

	_transformCBuffer->SetWorldMatrix(matWorld);
	_transformCBuffer->SetViewMatrix(matView);
	_transformCBuffer->SetProjMatrix(level->GetProjMatrix());
	_transformCBuffer->Update();

	_uiCBuffer->SetTint(_brush[_buttonState]._tint);

	if (_brush[_buttonState]._texture)
	{
		_uiCBuffer->SetTextureEnable(true);
		_brush[_buttonState]._texture->SetShader(0, SHADER_TYPE::PIXEL, 0);
	}
	else
	{
		_uiCBuffer->SetTextureEnable(false);
	}

	if (_brush[_buttonState]._animEnable)
	{
		_brush[_buttonState].SetShader(_uiCBuffer);
	}
	else
	{
		_uiCBuffer->SetAnimEnable(false);
	}

	_uiCBuffer->Update();
	_shader->SetShader();
	_mesh->Render();

	Widget::Render(pos);
}

Ptr<Widget> Button::MouseHit(const FVector2D& pos)
{
	return Widget::MouseHit(pos);
}

bool Button::IsInteractable()
{
	return (_buttonState != UI_BUTTON_STATE::DISABLE && _buttonState != UI_BUTTON_STATE::END);
}

void Button::OnHovered()
{
	if (!IsInteractable())
	{
		return;
	}

	_buttonState = UI_BUTTON_STATE::HOVERED;
	ExcuteEvent(UI_BUTTON_EVENT_STATE::HOVERED);
}

void Button::OnUnHovered()
{
	if (!IsInteractable())
	{
		return;
	}

	_buttonState = UI_BUTTON_STATE::ENABLE;
	ExcuteEvent(UI_BUTTON_EVENT_STATE::UNHOVERED);

}

void Button::OnClick()
{
	if (!IsInteractable())
	{
		return;
	}

	_buttonState = UI_BUTTON_STATE::ENABLE;
	ExcuteEvent(UI_BUTTON_EVENT_STATE::CLICKED);

}

void Button::OnPressed()
{
	if (!IsInteractable())
	{
		return;
	}

	_buttonState = UI_BUTTON_STATE::PRESSED;
	ExcuteEvent(UI_BUTTON_EVENT_STATE::PRESSED);

}

void Button::Destroy()
{
}

void Button::ExcuteEvent(UI_BUTTON_EVENT_STATE::Type type)
{
	if (_callBackFunc[type])
	{
		_callBackFunc[type]();
	}
}

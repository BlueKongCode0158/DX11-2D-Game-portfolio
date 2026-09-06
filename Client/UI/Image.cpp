#include "pch.h"
#include "Image.h"
#include "../Core/Device.h"
#include "../Common/LogManager.h"
#include "../World/Level.h"
#include "../Object/Asset/AssetManager.h"
#include "../Object/Asset/Animation/AnimationManager.h"
#include "../Object/Asset/Animation/Animation2DData.h"
#include "../Object/Asset/Animation/Animation2DSequence.h"
#include "../Object/Asset/Mesh/Mesh.h"

#include "../Shader/ShaderManager.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/UIShader.h"
#include "../Shader/ConstantBuffer/UICBuffer.h"

#include "../Object/Asset/Texture/Texture.h"


Image::Image()
{
}

Image::~Image()
{
}

void Image::SetTexture(const std::string& name)
{
    SetTexture(TEXTURE_MANAGER->FindTexture(name));
}

void Image::SetTexture(const std::string& name, const std::wstring& fileName)
{
    if (!TEXTURE_MANAGER->LoadTexture(name, fileName))
    {
        return;
    }
    SetTexture(TEXTURE_MANAGER->FindTexture(name));
}

void Image::SetTexture(Ptr<class Texture> texture)
{
    _brush._texture = texture;
}

void Image::SetOpacity(float op)
{
    _brush._tint._w = op;
}

void Image::SetTint(float r, float g, float b, float a)
{
    _brush._tint._x = r;
    _brush._tint._y = g;
    _brush._tint._z = b;
    _brush._tint._w = a;
}

void Image::SetBrushAnimEnable(bool enable)
{
    _brush._animEnable = enable;
}

void Image::AddBrushFrame(const FVector2D& start, const FVector2D& size)
{
    AddBrushFrame(start._x, start._y, size._x, size._y);
}

void Image::AddBrushFrame(float startX, float startY, float sizeX, float sizeY)
{
    FAnimationFrame frame;
    frame._start._x = startX;
    frame._start._y = startY;

    frame._size._x = sizeX;
    frame._size._y = sizeY;

    _brush._frames.push_back(frame);
}

void Image::SetCurrentFrame(int32 frame)
{
    _brush._sequence->SetFrame(frame);
}

void Image::SetAnimationPlayTime(float time)
{
    _brush._sequence->SetPlayTime(time);
}


void Image::SetBrushAnimation(const std::string& name)
{
    Ptr<Animation2DData> data = ANIMATION_MANAGER->FindAnimation(name);
    if (!data)
    {
        return;
    }
    _brush.SetSequence(name, data->GetLoop(), false, data->GetPlayTime(), _brush._playRate, data->GetStartLoopFrame());
}

void Image::SetBrushAnimationPlay(bool play)
{
    _brush._sequence->SetPlay(play);
}

void Image::SetBrushAnimationLoop(bool loop)
{
    _brush._sequence->SetLoop(loop);
}

void Image::SetSize(const FVector2D& size)
{
    _releativeSize._x = _brush._texture->GetTexture(0)->_width;
    _releativeSize._y = _brush._texture->GetTexture(0)->_height;
    _releativeSize.Normalize();

    _size._x = size._x * _releativeSize._x;
    _size._y = size._y * _releativeSize._y;
}

bool Image::Init(int32 id, const std::string& name, Weak<class Level> level)
{
    Widget::Init(id, name, level);

    return true;
}

void Image::Tick(float deltaTime)
{
    Widget::Tick(deltaTime);
    
    if (_brush._sequence)
    {
        _brush.Tick(deltaTime);
    }
}

void Image::Render()
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

    matScale.Scaling(_size);
    matTrans.Translation(_renderPos);

    matWorld = matScale * matRot * matTrans;

    FMatrix matView;
    _transformCBuffer->SetWorldMatrix(matWorld);
    _transformCBuffer->SetViewMatrix(matView);
    _transformCBuffer->SetProjMatrix(level->GetUIProjMatrix());
    _transformCBuffer->Update();

    FMatrix ScreenMatrix = matWorld * matView * level->GetUIProjMatrix();
    _uiCBuffer->SetTint(_brush._tint);

    if (_brush._texture)
    {
        _uiCBuffer->SetTextureEnable(true);
        _brush._texture->SetShader(0, SHADER_TYPE::PIXEL, 0);
    }
    else
    {
        _uiCBuffer->SetTextureEnable(false);
    }

    if (_brush._animEnable)
    {
        _brush.SetShader(_uiCBuffer);
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

void Image::Render(const FVector3D& pos)
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

    FResolution rs = Device::Instance().GetRS();
    const FVector3D& camPos = level->GetCameraWorldPos();

    FVector2D renderPos = _renderPos;
    renderPos._x += pos._x;
    renderPos._y += pos._y;

    renderPos._x -= camPos._x;
    renderPos._y -= camPos._y;

    ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
    FMatrix matScale, matRot, matTrans, matWorld;

    matScale.Scaling(_size);
    matTrans.Translation(renderPos);

    matWorld = matScale * matRot * matTrans;

    FMatrix matView;
    _transformCBuffer->SetWorldMatrix(matWorld);
    _transformCBuffer->SetViewMatrix(matView);
    _transformCBuffer->SetProjMatrix(level->GetProjMatrix());
    _transformCBuffer->Update();

    _uiCBuffer->SetTint(_brush._tint);

    if (_brush._texture)
    {
        _uiCBuffer->SetTextureEnable(true);
        _brush._texture->SetShader(0, SHADER_TYPE::PIXEL, 0);
    }
    else
    {
        _uiCBuffer->SetTextureEnable(false);
    }

    if (_brush._animEnable)
    {
        _brush.SetShader(_uiCBuffer);
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

void Image::Destroy()
{
}

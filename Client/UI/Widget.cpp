#include "pch.h"
#include "Widget.h"

#include "../Object/Actor.h"
#include "../Object/Asset/AssetManager.h"

#include "../Object/Asset/Mesh/Mesh.h"
#include "../Shader/ShaderManager.h"

#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Shader/ConstantBuffer/UICBuffer.h"

#include "../Object/Asset/Animation/Animation2DSequence.h"
#include "../Object/Asset/Animation/AnimationManager.h"
#include "../Object/Asset/Texture/Texture.h"

#include <algorithm>

void FUIBrush::SetSequence(const std::string& dataName, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	Ptr<Animation2DData> data = ANIMATION_MANAGER->FindAnimation(dataName);
	if (data)
	{
		return;
	}

	Ptr<Animation2DSequence> sq = New<Animation2DSequence>();
	sq->SetName(data->GetName());
	sq->SetData(data);
	sq->SetFrameRange(0, -1);
	sq->SetReverse(reverse);
	sq->SetLoop(loop);
	sq->SetPlayTime(playTime);
	sq->SetPlayRate(playRate);
	sq->SetStartLoopFrame(loopFrame);
	sq->SetPlayTime(0);

	if (!_sequence)
	{
		_sequence = sq;
		_texture = _sequence->GetAnimation2DDate()->GetTexture();
	}
	else
	{
		DESTROY(sq);
	}
}

void FUIBrush::Tick(float deltaTime)
{
	if (_sequence)
	{
		_sequence->Tick(deltaTime);
	}
}

void FUIBrush::SetShader(Ptr<class UICBuffer> cbuffer)
{
	cbuffer->SetAnimEnable(true);
	float LTX = 0.f;
	float LTY = 0.f;
	float RBX = 1.f;
	float RBY = 1.f;
	
	float offsetX = 0.f;
	float offsetY = 0.f;
	
	bool isRotated = false;
	bool isFlipped = false;

	if (Ptr<Animation2DData> anim = Lock<Animation2DData>(this->_sequence->GetAnimation2DDate()))
	{
	
	    const FAnimationFrame& frame = anim->GetFrame(this->_sequence->GetFrame());
	
	    Ptr<Texture> frameTexture = anim->GetFrameTexture(this->_sequence->GetFrame());
	    const FTextureInfo* info = frameTexture ? frameTexture->GetTexture(0) : nullptr;
	    if (info)
	    {
	        LTX = frame._start._x / info->_width;
	        LTY = frame._start._y / info->_height;
	
	        RBX = LTX + frame._size._x / info->_width;
	        RBY = LTY + frame._size._y / info->_height;
	    }
	
	    offsetX = frame._offset._x;
	    offsetY = frame._offset._y;
	
	    isRotated = frame._rotated90;
	    isFlipped = frame._facingFlip;
	}
	else
	{
	    int32 frame = this->_sequence->GetFrame();
	    FAnimationFrame frameData = this->_frames[frame];
		cbuffer->SetUV(frameData._start, frameData._start + frameData._size);
	}

	cbuffer->SetUV({ LTX, LTY }, { RBX, RBY });
	cbuffer->SetRotate(isRotated);
}

Widget::Widget()
{
}

Widget::~Widget()
{
}

Ptr<class Level> Widget::GetLevel()
{
	return Lock<Level>(_level);
}

int32 Widget::GetOwnerID()
{
	return _widgetID;
}

void Widget::SetOwnerActor(Weak<class Actor> owner)
{
	_ownerActor = owner;
}

Ptr<class Actor> Widget::GetOwnerActor() const
{
	return Lock<Actor>(_ownerActor);
}

void Widget::SetParent(Ptr<Widget> parent)
{
	_parent = parent;
}

Ptr<Widget> Widget::GetParent() const
{
	return Lock<Widget>(_parent);
}

const FVector2D& Widget::GetPos() const
{
	return _pos;
	// TODO: 여기에 return 문을 삽입합니다.
}

const FVector2D& Widget::GetRenderPos() const
{
	return _renderPos;
	// TODO: 여기에 return 문을 삽입합니다.
}

const FVector2D& Widget::GetSize() const
{
	return _size;
	// TODO: 여기에 return 문을 삽입합니다.
}

int16 Widget::GetOrder() const
{
	return _order;
}

void Widget::SetOrder(int16 order)
{
	_order = order;
}

void Widget::SetRenderPos(const FVector2D& renderPos)
{
	_renderPos = renderPos;
}

void Widget::SetPos(const FVector2D& pos)
{
	_pos = pos;

	for (auto& it : _children)
	{
		it->SetPos(pos);
	}
}

void Widget::SetSize(const FVector2D& size)
{
	_size = size;

	for (auto& it : _children)
	{
		it->SetSize(size);
	}

}

void Widget::RemoveChild(int32 childID)
{
	_removeWidgets.push_back(childID);
}

void Widget::AddChild(Ptr<Widget> child)
{
	_children.push_back(child);
}

void Widget::SetSorted(bool sort)
{
	_sorted = sort;
}

bool Widget::Init(int32 id, const std::string& name, Weak<class Level> level)
{
	_widgetID = id;
	_name = name;
	_level = level;

	_mesh = MESH_MANAGER->FindMesh("TexRect");
	_shader = FIND_SHADER("UIShader");
	_uiCBuffer = FIND_CBUFFER("UI", UICBuffer);
	_transformCBuffer = FIND_CBUFFER("Transform", TransformCBuffer);

	SetEnable(true);
	SetActive(true);
	return true;
}

void Widget::Tick(float deltaTime)
{
	if (auto parent = Lock<Widget>(_parent))
	{
		_renderPos = parent->GetRenderPos() + _pos;
	}
	else
	{
		_renderPos = _pos;
	}

	size_t size = _removeWidgets.size();
	for (size_t i= 0; i < size; i++)
	{
		auto it = _children.begin();
		for (; it != _children.end();)
		{
			if ((*it)->_widgetID == _removeWidgets[i])
			{
				DESTROY((*it));
				it = _children.erase(it);
				continue;
			}

			it++;
		}
	}
	_removeWidgets.clear();

	if (_sorted)
	{
		std::sort(_children.begin(), _children.end(), [](const Ptr<Widget>& w1, const Ptr< Widget>& w2)
			{
				return w1->_order < w2->_order;
			});
		SetSorted(false);
	}

	for (auto& it : _children)
	{
		it->Tick(deltaTime);
	}
}

void Widget::Render()
{
	for (auto& it : _children)
	{
		it->Render();
	}
}

void Widget::Render(const FVector3D& pos)
{
	for (auto& it : _children)
	{
		FVector3D childPos;
		childPos._x = pos._x + _pos._x;
		childPos._y = pos._y + _pos._y;
		childPos._z = 1.f;

		it->Render(childPos);
	}
}

Ptr<Widget> Widget::MouseHit(const FVector2D& pos)
{
	size_t size = _children.size();
	if(size > 0)
	{

		for (size_t i = size - 1; i > 0; i--)
		{
			Ptr<Widget> widget = _children[i];
			if (!widget->IsEnable() || !widget->IsActive())
			{
				continue;
			}

			Ptr<Widget> hit = widget->MouseHit(pos);
			if (hit)
			{
				return hit;
			}
		}
	}

	if (IsInteractable() && IsInSide(pos))
	{
		return This<Widget>();
	}
	else
	{
		return nullptr;
	}
}

bool Widget::IsInteractable()
{
	return false;
}

void Widget::OnHovered()
{
}

void Widget::OnUnHovered()
{
}

void Widget::OnClick()
{
}

void Widget::OnPressed()
{
}

void Widget::Destroy()
{
}

void Widget::SetAnimationPlayTime(float time)
{
}

void Widget::SetAnimationPlayRate(float rate)
{
}

void Widget::SetBrushAnimation(const std::string& name)
{
}

void Widget::SetBrushAnimationPlay(bool play)
{
}

void Widget::SetBrushAnimationLoop(bool loop)
{
}

bool Widget::IsInSide(const FVector2D& pos)
{
	FVector2D min, max;
	min._x = _renderPos._x - (_size._x / 2);
	min._y = _renderPos._y - (_size._y / 2);

	max._x = _renderPos._x + (_size._x / 2);
	max._y = _renderPos._y + (_size._y / 2);


	return pos._x >= min._x
		&& pos._x <= max._x
		&& pos._y >= min._y
		&& pos._y <= max._y;
}


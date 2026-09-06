#include "pch.h"
#include "Animation2D.h"
#include "SpriteComponent.h"
#include "../../Object/Asset/Animation/Animation2DSequence.h"
#include "../../Object/Asset/AssetManager.h"
#include "../../Object/Asset/Texture/Texture.h"

#include "../../Shader/ShaderManager.h"
#include "../../Object/Asset/Animation/Animation2DData.h"
#include "../../Shader/ConstantBuffer/AnimCBuffer.h"

#include "../../Editor/EditEngine.h"

Animation2D::Animation2D()
{
}

Animation2D::~Animation2D()
{
}

bool Animation2D::Init()
{
	_animBuffer = ShaderManager::Instance().FindCBuffer<AnimCBuffer>("Anim");
	return true;
}

void Animation2D::Tick(float deltaTime)
{
	if(_currentSequences)
		_currentSequences->Tick(deltaTime);
}

void Animation2D::Destroy()
{
	// TODO : DESTROY (ANIM2D)
}

void Animation2D::DrawInspector()
{
	ImGui::SeparatorText("Animation2D");

	std::vector<std::string> seqStr;
	for (auto& it : _sequences)
	{
		seqStr.push_back(it.second->GetName());
	}

	static int item_seleted_index = 0;
	int prev_index = item_seleted_index;

	if (ImGui::BeginCombo("AnimationSequence", seqStr[item_seleted_index].c_str()))
	{
		size_t size = seqStr.size();
		for (size_t index = 0; index < size; index++)
		{
			const bool is_selected = (item_seleted_index == index);
			if (ImGui::Selectable(seqStr[index].c_str(), is_selected))
			{
				item_seleted_index = index;
			}
			
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	const std::string seleteSeq = seqStr[item_seleted_index];

	Ptr<Animation2DSequence> foundSeq = FindSequence(seleteSeq);
	if (foundSeq)
	{
		foundSeq->DrawInspector();
		ChangeAnimation(seleteSeq);
	}
}

void Animation2D::SetAnimFlip(bool flip)
{
	_flip = flip;
}


void Animation2D::SetShader()
{
	float LTX = 0.f;
	float LTY = 0.f;
	float RBX = 1.f;
	float RBY = 1.f;
	
	float offsetX = 0.f;
	float offsetY = 0.f;

	bool isRotated = false;
	bool isFlipped = false;

	if(_currentSequences)
	{
		eAnimTextureType type = _currentSequences->_data->GetAnimationTextureType();
		Ptr<Texture> texture = _currentSequences->_data->GetTexture();

		Ptr<SpriteComponent> comp = Lock<SpriteComponent>(_owner);
		if (nullptr == comp)
		{
			return;
		}

		switch (type)
		{
		case eAnimTextureType::SPRITE:
		{
			const int32 curFrame = _currentSequences->GetFrame();
			const FAnimationFrame& frame = _currentSequences->_data->GetFrame(curFrame);

			// 프레임이 속한 아틀라스를 가져와 그 텍스처로 바인딩 + 그 크기로 UV 정규화.
			// (한 클립이 여러 아틀라스에 걸쳐 있을 수 있으므로 프레임마다 다를 수 있음)
			Ptr<Texture> frameTexture = _currentSequences->_data->GetFrameTexture(curFrame);
			if (nullptr == frameTexture)
			{
				frameTexture = texture;
			}

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

			comp->SetTexture(frameTexture, 0);
		}
		break;
		case eAnimTextureType::FRAME:
		{
			comp->SetTextureIndex(_currentSequences->_frame);
			offsetX = 0.f;
			offsetY = 0.f;
		}
		break;
		case eAnimTextureType::MESHSPRITE:
		{

		}
		case eAnimTextureType::ANIMATION_TEXTURE_END:
			break;
		default:
			break;
		}

	}
	_animBuffer->SetUV(LTX, LTY, RBX, RBY);
	_animBuffer->SetOffset(offsetX, offsetY);
	_animBuffer->SetAnimRotated(isRotated);
	_animBuffer->SetAnimFlip(_flip);
	_animBuffer->Update();
}

bool Animation2D::AddSequence(const std::string& name, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	return AddSequence(name, name, 0, -1, loop, reverse, playTime, playRate, loopFrame);
}

bool Animation2D::AddSequence(const std::string& sequenceName, const std::string& dataName, int32 startFrame, int32 frameCount, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	Ptr<Animation2DSequence> sq = FindSequence(sequenceName);
	if (nullptr != sq)
	{
		return false;
	}

	Ptr<Animation2DData> data = ANIMATION_MANAGER->FindAnimation(dataName);
	if (nullptr == data)
	{
		return false;
	}

	sq = New<Animation2DSequence>();
	sq->_owner = This<Animation2D>();
	sq->SetName(sequenceName);
	sq->SetData(data);
	sq->SetFrameRange(startFrame, frameCount);
	sq->SetReverse(reverse);
	sq->SetLoop(loop);
	sq->SetPlayTime(playTime);
	sq->SetPlayRate(playRate);
	sq->SetStartLoopFrame(loopFrame);
	sq->_time = 0;

	if (!_currentSequences)
	{
		_currentSequences = sq;
		if (Ptr<SpriteComponent> comp = Lock<SpriteComponent>(_owner))
		{
			comp->SetTexture(_currentSequences->GetAnimation2DDate()->GetTexture());
		}
	}
	_sequences[sequenceName] = sq;
	return true;
}

bool Animation2D::AddSequence(Ptr<class Animation2DData> data, bool loop, bool reverse, float playTime, float playRate, float loopFrame)
{
	Ptr<Animation2DSequence> sq = FindSequence(data->GetName());
	if (nullptr != sq)
	{
		return false;
	}

	sq = New<Animation2DSequence>();
	sq->_owner = This<Animation2D>();
	sq->SetName(data->GetName());
	sq->SetData(data);
	sq->SetFrameRange(0, -1);
	sq->SetReverse(reverse);
	sq->SetLoop(loop);
	sq->SetPlayTime(playTime);
	sq->SetPlayRate(playRate);
	sq->SetStartLoopFrame(loopFrame);
	sq->_time = 0;
	
	if (!_currentSequences)
	{
		_currentSequences = sq;
		if (Ptr<SpriteComponent> comp = Lock<SpriteComponent>(_owner))
		{
			comp->SetTexture(_currentSequences->GetAnimation2DDate()->GetTexture());
		}
	}
	_sequences[data->GetName()] = sq;
	return true;
}

bool Animation2D::AddSequenceAuto(const std::string& name, bool reverse, float playRate)
{
	Ptr<Animation2DData> data = ANIMATION_MANAGER->FindAnimation(name);
	if (nullptr == data)
	{
		return false;
	}

	// fps 가 baking 된 클립이면 그 속도/루프/루프지점을 그대로 사용하고,
	// (Player::* 처럼) fps 가 없는 클립은 기존 기본값으로 폴백한다.
	float playTime = data->GetPlayTime();
	if (playTime <= 0.f)
	{
		playTime = 1.f;
	}

	return AddSequence(name, data->GetLoop(), reverse, playTime, playRate,
		static_cast<float>(data->GetStartLoopFrame()));
}

void Animation2D::SetPlayTime(const std::string& name, float playTime)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetPlayTime(playTime);
}

void Animation2D::SetPlayRate(const std::string& name, float playRate)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetPlayRate(playRate);
}

void Animation2D::SetLoop(const std::string& name, bool loop)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetLoop(loop);
}

void Animation2D::SetReverse(const std::string& name, bool reverse)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetReverse(reverse);
}

void Animation2D::SetPlay(const std::string& name, bool play)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetPlay(play);
}

void Animation2D::SetLoopAnimationFrame(const std::string& name, int32 loopFrame)
{
	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}
	sq->SetStartLoopFrame(loopFrame);
}

void Animation2D::ChangeAnimation(const std::string& name)
{
	if (!_currentSequences)
	{
		return;
	}

	if (_sequences.size() < 1)
	{
		return;
	}

	//if (_currentSequences->GetName() == name)
	//{
	//	return;
	//}

	Ptr<Animation2DSequence> sq = FindSequence(name);
	if (nullptr == sq)
	{
		return;
	}

	// 이전 상태 초기화
	_currentSequences->_frame = 0;
	_currentSequences->_time = 0;

	/// 바꿀 시퀀스를 현재 시퀀스로 지정.
	_currentSequences = sq;
	_currentSequences->_frame = 0;
	_currentSequences->_time = 0;
	_currentSequences->SetPlay(true);

	Ptr<SpriteComponent> comp = Lock<SpriteComponent>(_owner);
	if (nullptr != comp)
	{
		comp->SetTexture(_currentSequences->GetAnimation2DDate()->GetTexture());
	}

}

Ptr<class Animation2DData> Animation2D::GetCurrentSequneceData()
{
	if (!_currentSequences)
	{
		return nullptr;
	}
	return _currentSequences->GetAnimation2DDate();
}

void Animation2D::SetPlayCurAnimation(bool play)
{
	_currentSequences->SetPlay(play);
}

bool Animation2D::GetAnimFlip() const
{
	return _flip;
}

void Animation2D::AddFrameSize(float deltaTime)
{
	if (_currentSequences)
	{
		_currentSequences->AddFrameSize(deltaTime);
	}
}

Ptr<Animation2DSequence> Animation2D::FindSequence(const std::string& name)
{
	auto it = _sequences.find(name);
	if (_sequences.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

Ptr<Animation2DSequence> Animation2D::GetCurSequence()
{
	return _currentSequences;
}

bool Animation2D::IsAnimationEnd()
{
	return _currentSequences->GetPlay();
}

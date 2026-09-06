#include "pch.h"
#include "AnimationUI.h"
#include "../EditEngine.h"

#include <algorithm>

#include "../../Object/Asset/AssetManager.h"
#include "../../Object/Asset/Animation/AnimationManager.h"
#include "../../Object/Asset/Animation/Animation2DData.h"
#include "../../Object/Asset/Animation/Animation2DSequence.h"
#include "../../Object/Asset/Texture/Texture.h"

AnimationUI::AnimationUI()
{
}

AnimationUI::~AnimationUI()
{
}

bool AnimationUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(true);
	return true;
}

void AnimationUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	ImGui::Begin(_name.c_str());

	Ptr<AnimationManager> animMgr = ANIMATION_MANAGER;
	if (nullptr == animMgr)
	{
		ImGui::Text("AnimationManager is Null");
		ImGui::End();
		return;
	}

	const auto& anims = animMgr->GetAnimations();
	if (anims.empty())
	{
		ImGui::Text("No animation loaded.");
		ImGui::End();
		return;
	}

	// 이름순 정렬된 클립 목록
	if (anims.size() != _cachedAnimCount)
	{
		_sortedAnimNames.clear();
		_sortedAnimNames.reserve(anims.size());
		for (const auto& it : anims)
		{
			_sortedAnimNames.push_back(it.first);
			std::sort(_sortedAnimNames.begin(), _sortedAnimNames.end());
			_cachedAnimCount = anims.size();
		}
	}

	// 등록된 모든 애니메이션 목록
	const char* preview = _selectedName.empty() ? "Select Animation" : _selectedName.c_str();
	if (ImGui::BeginCombo("Animation", preview))
	{
		for (const std::string& name : _sortedAnimNames)
		{
			const bool selected = (name == _selectedName);
			if (ImGui::Selectable(name.c_str(), selected))
			{
				SelectAnimation(name);
			}
			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (_preview)
	{
		_preview->Tick(deltaTime);

		DrawControl();
		DrawPreview();
	}

	ImGui::End();
}

void AnimationUI::SelectAnimation(const std::string& name)
{
	if (name == _selectedName && _preview)
	{
		return;
	}

	Ptr<AnimationManager> animMgr = ANIMATION_MANAGER;
	if (nullptr == animMgr)
	{
		return;
	}

	Ptr<Animation2DData> data = animMgr->FindAnimation(name);
	if (nullptr == data)
	{
		return;
	}

	if (_preview)
	{
		DESTROY(_preview);
	}

	// 미리보기 시퀀스는 SpriteComponent/Animation2D 없이 단독으로 동작한다.
	// (Tick 은 _data 만 사용하므로 owner 가 필요없다)
	// 재생 속도/루프 정보는 원본 클립에서 baking 된 값을 그대로 사용한다.
	const float playTime = data->GetPlayTime();	// frame_count / fps (fps<=0 이면 0)

	_preview = New<Animation2DSequence>();
	_preview->SetData(data);
	_preview->SetStartLoopFrame(data->GetStartLoopFrame());
	_preview->SetPlayRate(1.f);
	_preview->SetPlayTime(playTime > 0.f ? playTime : 1.f);	// SetData 이후에 호출
	_preview->SetReverse(false);
	_preview->SetLoop(data->GetLoop());
	_preview->SetPlay(true);

	_selectedName = name;
}

void AnimationUI::DrawControl()
{
	ImGui::SeparatorText("Control");

	// Play / Reverse / Loop / PlayTime / PlayRate 는 시퀀스가 이미 그려준다.
	_preview->DrawInspector();

	Ptr<Animation2DData> data = _preview->GetAnimation2DDate();
	if (nullptr == data)
	{
		return;
	}

	int32 frameCount = data->GetFrameCount();
	if (frameCount <= 0)
	{
		return;
	}

	int32 frame = _preview->GetFrame();
	if (ImGui::SliderInt("Frame", &frame, 0, frameCount - 1))
	{
		_preview->SetFrame(frame);
	}
	ImGui::Text("Frame : %d / %d", frame, frameCount - 1);

	ImGui::Checkbox("Flip", &_flip);	// gAnim2DFlip 재현

	// 원본 클립에서 baking 된 재생 정보 (참고용)
	ImGui::SeparatorText("Clip Info");
	ImGui::Text("fps        : %.2f", data->GetFps());
	ImGui::Text("frames     : %d", frameCount);
	ImGui::Text("loop       : %s", data->GetLoop() ? "true" : "false");
	ImGui::Text("loop start : %d", data->GetStartLoopFrame());
	ImGui::Text("play time  : %.3f s", data->GetPlayTime());
}

// HLSL Share.fx 의 UpdateAnimation2D 를 그대로 재현한다.
// mesh 쿼드 UV(0~1) → 아틀라스 샘플 UV. rotated90 이면 쿼드 UV 를 90도 회전시킨다.
static ImVec2 ShaderAtlasUV(const ImVec2& meshUV, bool rotated, const ImVec2& lt, const ImVec2& rb)
{
	float u = meshUV.x;
	float v = meshUV.y;

	if (rotated)
	{
		const float ou = u;
		const float ov = v;
		u = 1.f - ov;	// uv = float2(1 - UV.y, ...)
		v = 1.f - ou;	// ...        1 - UV.x )
	}

	return ImVec2(
		lt.x + (rb.x - lt.x) * u,
		lt.y + (rb.y - lt.y) * v
	);
}

void AnimationUI::DrawPreview()
{
	ImGui::SeparatorText("Preview");
	ImGui::DragFloat("Scale", &_previewScale, 0.05f, 0.1f, 8.f);

	Ptr<Animation2DData> data = _preview->GetAnimation2DDate();
	if (nullptr == data)
	{
		return;
	}

	Ptr<Texture> texture = data->GetTexture();
	if (nullptr == texture)
	{
		ImGui::Text("Texture is Null");
		return;
	}

	// 프레임이 없는 클립은 GetFrame() 이 빈 vector 에 접근하므로 반드시 막는다.
	if (data->GetFrameCount() <= 0)
	{
		ImGui::Text("No frames in this clip.");
		return;
	}

	const int32 frame = _preview->GetFrame();
	const eAnimTextureType type = data->GetAnimationTextureType();

	ImVec2 lt(0.f, 0.f);			// gAnim2DLTUV
	ImVec2 rb(1.f, 1.f);			// gAnim2DRBUV
	bool rotated = false;			// gAnim2DRotated90
	int32 texIndex = 0;
	ImVec2 drawSize(0.f, 0.f);		// 화면에 표시할 정방향 픽셀 크기 (= frame._draw)
	ImVec2 offset(0.f, 0.f);		// 피봇 기준 오프셋 (월드 단위)
	ImVec2 canvas(0.f, 0.f);		// 정렬 기준 캔버스(원본) 크기
	Ptr<Texture> drawTexture = texture;	// 실제로 그릴 텍스처 (SPRITE 는 프레임별 아틀라스)

	switch (type)
	{
	case eAnimTextureType::SPRITE:
	{
		// 프레임이 속한 아틀라스를 가져온다 (멀티 텍스처 클립 지원). 인덱스 0 에 한 장.
		drawTexture = data->GetFrameTexture(frame);
		if (nullptr == drawTexture)
		{
			drawTexture = texture;
		}

		const FTextureInfo* info = drawTexture->GetTexture(0);
		if (nullptr == info)
		{
			return;
		}

		const float tw = static_cast<float>(info->_width);
		const float th = static_cast<float>(info->_height);
		if (tw <= 0.f || th <= 0.f)
		{
			return;
		}

		const FAnimationFrame& f = data->GetFrame(frame);
		lt = ImVec2(f._start._x / tw, f._start._y / th);
		rb = ImVec2((f._start._x + f._size._x) / tw, (f._start._y + f._size._y) / th);
		rotated = f._rotated90;

		// 표시 크기: draw 가 있으면 그대로(이미 정방향), 없으면 atlas size(회전 시 swap)
		if (f._draw._x != 0.f && f._draw._y != 0.f)
		{
			drawSize = ImVec2(f._draw._x, f._draw._y);
		}
		else
		{
			drawSize = rotated ? ImVec2(f._size._y, f._size._x) : ImVec2(f._size._x, f._size._y);
		}

		offset = ImVec2(f._offset._x, f._offset._y);
		canvas = ImVec2(f._canvasSize._x, f._canvasSize._y);
		texIndex = 0;
	}
	break;
	case eAnimTextureType::FRAME:
	{
		// 프레임마다 별도의 텍스쳐 인덱스를 사용 → 전체 UV
		texIndex = frame;
		const FTextureInfo* info = texture->GetTexture(texIndex);
		if (nullptr == info)
		{
			return;
		}
		drawSize = ImVec2(static_cast<float>(info->_width), static_cast<float>(info->_height));
	}
	break;
	default:
	{
		// MESHSPRITE 등은 0번 텍스쳐 전체를 보여주는 것으로 대체
		const FTextureInfo* info = texture->GetTexture(0);
		if (nullptr == info)
		{
			return;
		}
		drawSize = ImVec2(static_cast<float>(info->_width), static_cast<float>(info->_height));
		texIndex = 0;
	}
	break;
	}

	const FTextureInfo* drawInfo = drawTexture->GetTexture(texIndex);
	if (nullptr == drawInfo || nullptr == drawInfo->_srv)
	{
		return;
	}

	if (drawSize.x <= 0.f || drawSize.y <= 0.f)
	{
		return;
	}

	const float s = _previewScale;
	const float halfX = drawSize.x * s * 0.5f;
	const float halfY = drawSize.y * s * 0.5f;

	// 프레임이 offset 만큼 움직여도 잘리지 않도록 영역(피봇 중심)을 잡는다.
	// 캔버스가 있으면 캔버스를 기준 영역으로 쓰고, 프레임이 더 크면 그만큼 넓힌다.
	const float absOffX = (offset.x < 0.f ? -offset.x : offset.x) * s;
	const float absOffY = (offset.y < 0.f ? -offset.y : offset.y) * s;
	float regionHalfX = (canvas.x > 0.f ? canvas.x * s * 0.5f : halfX);
	float regionHalfY = (canvas.y > 0.f ? canvas.y * s * 0.5f : halfY);
	if (absOffX + halfX > regionHalfX) regionHalfX = absOffX + halfX;
	if (absOffY + halfY > regionHalfY) regionHalfY = absOffY + halfY;

	// 레이아웃 공간을 확보하고, 그 영역의 중심을 피봇(origin) 으로 삼는다.
	const ImVec2 regionMin = ImGui::GetCursorScreenPos();
	ImGui::Dummy(ImVec2(regionHalfX * 2.f, regionHalfY * 2.f));
	const ImVec2 origin(regionMin.x + regionHalfX, regionMin.y + regionHalfY);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// 정렬 기준을 눈으로 확인할 수 있게 캔버스 외곽선 + 피봇 십자선 표시
	if (canvas.x > 0.f && canvas.y > 0.f)
	{
		const ImVec2 cmin(origin.x - canvas.x * s * 0.5f, origin.y - canvas.y * s * 0.5f);
		const ImVec2 cmax(origin.x + canvas.x * s * 0.5f, origin.y + canvas.y * s * 0.5f);
		drawList->AddRect(cmin, cmax, IM_COL32(90, 90, 90, 255));
	}
	drawList->AddLine(ImVec2(origin.x - 5.f, origin.y), ImVec2(origin.x + 5.f, origin.y), IM_COL32(255, 60, 60, 220));
	drawList->AddLine(ImVec2(origin.x, origin.y - 5.f), ImVec2(origin.x, origin.y + 5.f), IM_COL32(255, 60, 60, 220));

	// ApplyAnimationFrameTransform 과 동일: 쿼드 중심 = 피봇 + offset.
	// 화면은 Y-down 이므로 offset.y 부호를 반전한다.
	const ImVec2 center(origin.x + offset.x * s, origin.y - offset.y * s);
	ImVec2 pos[4] = {
		ImVec2(center.x - halfX, center.y - halfY),	// TL
		ImVec2(center.x + halfX, center.y - halfY),	// TR
		ImVec2(center.x + halfX, center.y + halfY),	// BR
		ImVec2(center.x - halfX, center.y + halfY),	// BL
	};

	// gAnim2DFlip : 지오메트리 X 반전 + offset.x 반전을, 피봇(origin.x) 기준
	// 좌우 미러 한 번으로 동시에 재현한다.
	if (_flip)
	{
		for (int32 i = 0; i < 4; ++i)
		{
			pos[i].x = 2.f * origin.x - pos[i].x;
		}
	}

	// 각 코너의 mesh UV 를 셰이더와 동일하게 아틀라스 UV 로 변환
	const ImVec2 meshUV[4] = { ImVec2(0.f, 0.f), ImVec2(1.f, 0.f), ImVec2(1.f, 1.f), ImVec2(0.f, 1.f) };
	ImVec2 uv[4];
	for (int32 i = 0; i < 4; ++i)
	{
		uv[i] = ShaderAtlasUV(meshUV[i], rotated, lt, rb);
	}

	drawList->AddImageQuad(
		reinterpret_cast<void*>(drawInfo->_srv.Get()),
		pos[0], pos[1], pos[2], pos[3],
		uv[0], uv[1], uv[2], uv[3]);
}

void AnimationUI::Destroy()
{
	if (_preview)
	{
		DESTROY(_preview);
	}
	EditorUI::Destroy();
}

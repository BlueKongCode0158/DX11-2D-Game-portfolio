#pragma once
#include "EditorUI.h"

/*
씬에 배치된 액터와 무관하게, AnimationManager 에 등록된 모든 애니메이션을
골라서 재생/스크럽/미리보기 할 수 있는 테스트 전용 패널.
미리보기는 실제 렌더 파이프라인을 거치지 않고, 현재 프레임을
텍스쳐 SRV + UV 로 ImGui::Image 에 직접 그려준다.
*/
class AnimationUI : public EditorUI
{
public:
	AnimationUI();
	virtual ~AnimationUI();
public:
	virtual bool Init(const std::string& name) override;
	virtual void Render(float deltaTime) override;
	virtual void Destroy() override;
private:
	void SelectAnimation(const std::string& name);	// 콤보에서 애니 선택 시 미리보기 시퀀스 구성
	void DrawControl();								// Play/Reverse/Loop/PlayTime/PlayRate + 프레임 스크럽
	void DrawPreview();								// 현재 프레임 이미지 출력
private:
	Ptr<class Animation2DSequence> _preview;		// 미리보기용 시퀀스 (소유자 없음)
	std::string _selectedName;
	float _previewScale = 1.f;
	bool _flip = false;								// gAnim2DFlip 와 동일하게 좌우 반전 테스트
private:
	std::vector < std::string > _sortedAnimNames;
	size_t _cachedAnimCount = 0;
};

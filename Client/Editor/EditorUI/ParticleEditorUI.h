#pragma once
#include "EditorUI.h"

/*
현재 Inspector 가 가리키는 ParticleComponent 를 대상으로 프리셋 적용 /
재생 제어 / 속성 편집을 제공하는 전용 패널.
속성 편집 UI 는 Inspector 와 동일한 DrawParticleEmitPropertyUI() 헬퍼를 재사용한다.
편집 결과는 씬에 배치된 실제 파티클에 즉시 반영되므로 메인 뷰포트에서 바로 확인된다.
*/
class ParticleEditorUI : public EditorUI
{
public:
	ParticleEditorUI();
	virtual ~ParticleEditorUI();
public:
	virtual bool Init(const std::string& name) override;
	virtual void Render(float deltaTime) override;
	virtual void Destroy() override;
private:
	// 현재 Inspector 타깃이 ParticleComponent 이면 반환, 아니면 nullptr.
	Ptr<class ParticleComponent> GetTargetParticle() const;
	// 선택한 파티클을 전용 직교 카메라로 오프스크린 RT 에 렌더해 패널에 표시.
	void DrawPreview(Ptr<class ParticleComponent> particle, float deltaTime);
private:
	Ptr<class ViewportRenderTarget> _previewRT;
	float _previewZoom = 1.f;
};

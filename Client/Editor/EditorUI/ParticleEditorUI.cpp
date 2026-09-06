#include "pch.h"
#include "ParticleEditorUI.h"
#include "../EditEngine.h"
#include "InspectorUI.h"
#include "ViewportRenderTarget.h"

#include "../../Component/ParticleComponent.h"
#include "../../Core/Device.h"
#include "../../Shader/ShaderManager.h"
#include "../../Shader/ConstantBuffer/TransformCBuffer.h"

ParticleEditorUI::ParticleEditorUI()
{
}

ParticleEditorUI::~ParticleEditorUI()
{
}

bool ParticleEditorUI::Init(const std::string& name)
{
	EditorUI::Init(name);
	SetEnable(true);

	_previewRT = New<ViewportRenderTarget>();
	_previewRT->Init(360, 360);
	return true;
}

Ptr<ParticleComponent> ParticleEditorUI::GetTargetParticle() const
{
	Ptr<InspectorUI> inspector = EditEngine::Instance().FindEditorUI<InspectorUI>("Inspector");
	if (nullptr == inspector)
	{
		return nullptr;
	}

	// Inspector 타깃이 ParticleComponent 일 때만 캐스팅 성공.
	return Cast<Object, ParticleComponent>(inspector->GetTarget());
}

void ParticleEditorUI::Render(float deltaTime)
{
	EditorUI::Render(deltaTime);

	ImGui::Begin(_name.c_str());

	Ptr<ParticleComponent> particle = GetTargetParticle();
	if (nullptr == particle)
	{
		ImGui::Text("Select a ParticleComponent in the OutLiner.");
		ImGui::End();
		return;
	}

	// 프리셋 갤러리 : 선택한 컴포넌트에 즉시 적용
	ImGui::SeparatorText("Preset");
	if (ImGui::Button("Smoke"))
	{
		particle->SetEmitProperty(MakeSmokePreset());
	}
	ImGui::SameLine();
	if (ImGui::Button("Dust"))
	{
		particle->SetEmitProperty(MakeDustPreset());
	}
	ImGui::SameLine();
	if (ImGui::Button("Fire"))
	{
		particle->SetEmitProperty(MakeFirePreset());
	}
	ImGui::SameLine();
	if (ImGui::Button("WhiteParticle"))
	{
		particle->SetEmitProperty(MakeWhitePalacePreset());
	}

	// 전용 직교 카메라로 격리된 미리보기 (편집 결과가 곧바로 반영된다)
	DrawPreview(particle, deltaTime);

	// 재생 제어 + 속성 편집은 컴포넌트가 Inspector 와 동일하게 그려준다.
	particle->DrawInspector();

	ImGui::End();
}

void ParticleEditorUI::DrawPreview(Ptr<ParticleComponent> particle, float deltaTime)
{
	if (nullptr == _previewRT)
	{
		return;
	}

	ImGui::SeparatorText("Preview");
	ImGui::DragFloat("Zoom", &_previewZoom, 0.05f, 0.1f, 20.f);

	const FParticleEmitProperty& prop = particle->GetEmitProperty();

	// 데이터 기반 자동 프레이밍 : 분사 도달거리 + 최대 크기를 담을 만큼 화면을 잡는다.
	// (_previewZoom 이 작을수록 확대) 절반 폭 half 를 직교 투영 범위로 사용.
	float reach = prop._speed._max * prop._lifeTime._max;
	float half = reach + prop._size._max + 1.f;
	if (half < 2.f)
	{
		half = 2.f;
	}
	half *= _previewZoom;

	// 파티클은 (moveWithTransform 여부와 무관하게) 결국 emitter 의 월드 위치 근처에
	// 렌더되므로, 카메라를 그 위치에 맞춰 +Z 를 바라보게 둔다.
	FVector3D center = particle->GetWorldPosition();

	FMatrix view;
	view.Translation(-center._x, -center._y, 100.f);

	const float viewDistance = 10000.f;
	FMatrix proj = DirectX::XMMatrixOrthographicOffCenterLH(-half, half, -half, half, 0.f, viewDistance);

	// 프리뷰 전용 카메라를 컴포넌트에 오버라이드로 넘긴다. Render() 가 레벨 카메라
	// 대신 이 view/proj 로 그린다. (world 행렬과 Update 는 Render() 가 직접 수행)
	particle->SetCameraOverride(view, proj);

	const float clear[4] = { 0.05f, 0.05f, 0.05f, 1.f };
	_previewRT->Begin(clear);

	particle->Render(deltaTime);

	// 프리뷰가 끝났으니 오버라이드를 해제해 게임/뷰포트 렌더는 레벨 카메라를 쓰게 한다.
	particle->ClearCameraOverride();

	// 백버퍼 타깃/뷰포트 복구 (이후 ImGui 렌더가 정상 진행되도록)
	Device::Instance().SetBackBufferTarget();

	float avail = ImGui::GetContentRegionAvail().x;
	float side = (avail > 16.f) ? avail : static_cast<float>(_previewRT->GetWidth());
	ImGui::Image(reinterpret_cast<ImTextureID>(_previewRT->GetSRV().Get()), ImVec2(side, side));
}

void ParticleEditorUI::Destroy()
{
	if (_previewRT)
	{
		DESTROY(_previewRT);
	}
	EditorUI::Destroy();
}

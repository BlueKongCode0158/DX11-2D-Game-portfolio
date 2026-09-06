#pragma once
#include "SceneComponent.h"

class FogPlaneComponent : public SceneComponent
{
public:
	FogPlaneComponent();
	virtual ~FogPlaneComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Render(float deltaTime) override;
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	void SetTint(FVector4D tint);
	void SetTint(float r, float g, float b, float w);
	void SetOp(float w);
private:
	Ptr<class Shader> _shader;
	Ptr<class Mesh> _mesh;
	Ptr<class FogPlaneCBuffer> _fogPlaneCBuffer;
	Ptr<class Texture> _noiseTexture;
	FVector4D _tint = FVector4D(1.f, 1.f, 1.f, 1.f);
private:
	float _time = 0.f;
	float _dist = 0.f;
};
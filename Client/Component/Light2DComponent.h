#pragma once
#include "SceneComponent.h"

struct FLightRenderProxy
{
	FMatrix				_world;
	Ptr<class Mesh>		_mesh;
	Ptr<class Texture>	_texture;

	int32		_textureIndex = 0;
	float		_intensity = 1.f;
	FVector4D	_tint = FVector4D(1.f, 1.f, 1.f, 1.f);
};

class Light2DComponent : public SceneComponent
{
public:
	Light2DComponent();
	virtual ~Light2DComponent();
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime) override;
	virtual void Destroy() override;
public:
	void SetMesh(const std::string& meshName, int32 pathID);
	void SetTexture(const std::string& textureName, int32 textureID);
	void SetTint(const FVector4D& tint);
	void SetIntensity(float intensity);
	bool BuildLightRenderProxy(OUT FLightRenderProxy& proxy) const;
public:
	virtual bool BuildRenderBound2D(OUT FLocalBound2D& out) const;
private:
	Ptr<class Mesh>		_mesh;
	Ptr<class Texture>	_texture;
	int32				_textureIndex = 0;
	FVector4D			_tint = FVector4D(1.f, 1.f, 1.f, 1.f);
	float				_intensity = 1.f;
};
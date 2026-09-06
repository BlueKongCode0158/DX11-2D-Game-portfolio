#pragma once
#include "Actor.h"

class LightActor : public Actor
{
public:
	LightActor();
	virtual ~LightActor();
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Destroy() override;
public:
	void SetMesh(const std::string& meshName, int32 pathID);
	void SetTexture(const std::string& textureName, int32 textureID);
	void SetTint(const FVector4D& tint);
	void SetIntensity(float intensity);
private:
	Ptr<class Light2DComponent> _meshComp;
};
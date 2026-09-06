#pragma once
#include "Actor.h"

class Prob : public Actor
{
public:
	Prob();
	virtual ~Prob();
protected:
	FLocalBound2D _worldBound;
	Ptr<class SpriteComponent> _meshComp;
	Ptr<class ParallaxLayerComponent> _parallaxComp;
public:
	virtual bool Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name);
	virtual void Tick(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void Collision(float deltaTime);
public:
	virtual void Destroy() override;
public:
	virtual void DrawInspector() override;
public:
	virtual bool Save(std::ofstream& file) override;
	virtual bool Load(std::ifstream& file) override;
public:
	void SetMesh(const std::string& mesh, const int32 pathID);
	void SetTexture(const std::string& AtlasName, int index = 0);
	void SetLayerName(const std::string& LayerName);
	void SetTint(const FVector4D& tint);
	void SetShader(const std::string& name);
	void SetBlendMode(eBlendMode eMode);
	// probInstance.json 의 부모(root) 월드 위치 → 패럴렉스 스케일 계산용
	void SetParallaxParentPosition(const FVector3D& parentPos);
public:
	void SetAnimation(const std::string& AnimationName);
private:
	Ptr<class SpriteComponent> CreateProbMesh();
protected:
	bool _isInteractive = false;
};
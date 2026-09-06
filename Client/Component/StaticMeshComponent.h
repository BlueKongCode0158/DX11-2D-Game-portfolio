#pragma once
#include "MeshComponent.h"

class StaticMeshComponent : public MeshComponent
{
public:
	StaticMeshComponent();
	virtual ~StaticMeshComponent();
protected:
	Weak<class Mesh> _mesh;
	Weak<class Shader> _shader;
public:
	void SetMesh(const std::string& name);
	void SetMesh(const std::string& AtlasMesh, int32 pathID);
	void SetShader(const std::string& name);
	void SetMaterial(int32 slotIndex, const std::string& name);
	void AddTexture(int32 slotIndex, const std::string& name, int32 registerNum, int32 shaderBufferType = SHADER_TYPE::PIXEL, int32 textureIndex = 0);
	void SetBaseColor(int32 slotIndex, float r, float g, float b, float a);
	void SetOpacity(int slotIndex, float opacity);
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner) override;
	virtual void Tick(float deltaTime) override;
	virtual void Collision(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
public:
	virtual bool BuildRenderBound2D(OUT FLocalBound2D& out) const;
};
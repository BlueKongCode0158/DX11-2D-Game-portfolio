#include "pch.h"
#include "MeshComponent.h"
#include "Asset/AssetManager.h"
#include "Shader/Shader.h"
#include "Shader/ShaderManager.h"
#include "Object/Asset/Mesh/Mesh.h"
#include "Shader/ConstantBuffer/TransformCBuffer.h"
#include "World/Level.h"
#include "../Object/Asset/Material/Material.h"

MeshComponent::MeshComponent()
{
}

MeshComponent::~MeshComponent()
{
}

bool MeshComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	if (false == SceneComponent::Init(id, name, owner))
	{
		return false;
	}
	_type = eComponentType::MESH;
	return true;
}

void MeshComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
}

void MeshComponent::Collision(float deltaTime)
{
	SceneComponent::Collision(deltaTime);
}

void MeshComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);

	Ptr<Level> level = Lock<Level>(_level);
	if (nullptr == level)
	{
		return;
	}
	FMatrix view = level->GetViewMatrix();
	FMatrix proj = level->GetProjMatrix();
	_transformCBuffer->SetWorldMatrix(_matrix._world);
	_transformCBuffer->SetProjMatrix(proj);
	_transformCBuffer->SetViewMatrix(view);
	_transformCBuffer->Update();

}

void MeshComponent::Destroy()
{
	SceneComponent::Destroy();
	for (auto& it : _materialSlots)
	{
		DESTROY(it);
	}
}

bool MeshComponent::Save(std::ofstream& file)
{
	return true;
}

bool MeshComponent::Load(std::ifstream& file)
{
	return true;
}

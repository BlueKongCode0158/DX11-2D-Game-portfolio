#include "pch.h"
#include "StaticMeshComponent.h"
#include "../Object/Asset/AssetManager.h"
#include "../Object/Asset/Mesh/Mesh.h"
#include "../Object/Asset/Material/Material.h"
#include "../Shader/ShaderManager.h"


StaticMeshComponent::StaticMeshComponent()
{
	_isRender = true;
}

StaticMeshComponent::~StaticMeshComponent()
{
}

void StaticMeshComponent::SetMesh(const std::string& name)
{
	Ptr<Mesh> foundMesh = MESH_MANAGER->FindMesh(name);
	if (nullptr == foundMesh)
	{
		return;
	}

	_mesh = foundMesh;
	_materialSlots.clear();

	size_t slotCount = foundMesh->GetSlotCount();

	for (size_t i = 0; i < slotCount; i++)
	{
		const FMeshSlot* slot = foundMesh->GetSlot(i);
		_materialSlots.emplace_back(slot->_material->Clone());
	}
	
}

void StaticMeshComponent::SetMesh(const std::string& AtlasMesh, int32 pathID)
{
	Ptr<Mesh> foundMesh = MESH_MANAGER->FindMesh(AtlasMesh, pathID);
	if (nullptr == foundMesh)
	{
		return;
	}

	_mesh = foundMesh;
	_materialSlots.clear();

	size_t slotCount = foundMesh->GetSlotCount();
	for (size_t i = 0; i < slotCount; i++)
	{
		const FMeshSlot* slot = foundMesh->GetSlot(i);
		_materialSlots.emplace_back(slot->_material->Clone());
	}

}

void StaticMeshComponent::SetShader(const std::string& name)
{
	_shader = ShaderManager::Instance().FindShader(name);
}

void StaticMeshComponent::SetMaterial(int32 slotIndex, const std::string& name)
{
	_materialSlots[slotIndex] = MATERIAL_MANAGER->CreateMaterialInstance(name);
}

void StaticMeshComponent::AddTexture(int32 slotIndex, const std::string& name, int32 registerNum, int32 shaderBufferType, int32 textureIndex)
{
	_materialSlots[slotIndex]->AddTexture(name, registerNum, shaderBufferType, textureIndex);
}

void StaticMeshComponent::SetBaseColor(int32 slotIndex, float r, float g, float b, float a)
{
	_materialSlots[slotIndex]->SetBaseColor(r, g, b, a);
}

void StaticMeshComponent::SetOpacity(int slotIndex, float opacity)
{
	_materialSlots[slotIndex]->SetOpacity(opacity);
}

bool StaticMeshComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	MeshComponent::Init(id, name, owner);
	SetShader("StaticMeshShader");
	_type = eComponentType::STATICMESH;
	return true;
}

void StaticMeshComponent::Tick(float deltaTime)
{
	MeshComponent::Tick(deltaTime);
}

void StaticMeshComponent::Collision(float deltaTime)
{
	MeshComponent::Collision(deltaTime);
}

void StaticMeshComponent::Render(float deltaTime)
{
	MeshComponent::Render(deltaTime);
	Ptr<Shader> shader = Lock<Shader>(_shader);
	if (shader)
	{
		shader->SetShader();
	}

	Ptr<Mesh> mesh = Lock<Mesh>(_mesh);
	if (mesh)
	{
		size_t slotCount = mesh->GetSlotCount();
		for (size_t i = 0; i < slotCount; i++)
		{
			_materialSlots[i]->SetMaterial();
			mesh->Render();
			_materialSlots[i]->ResetMaterial();
		}
	}
}

void StaticMeshComponent::Destroy()
{
	MeshComponent::Destroy();
}

bool StaticMeshComponent::Save(std::ofstream& file)
{
	return true;
}

bool StaticMeshComponent::Load(std::ifstream& file)
{
	return true;
}

bool StaticMeshComponent::BuildRenderBound2D(OUT FLocalBound2D& out) const
{
	Ptr<Mesh> mesh = Lock<Mesh>(_mesh);
	if (!mesh) return false;
	FLocalBound2D local = mesh->GetLocalBound2D();   // 메시 로컬 AABB (없으면 정점에서 계산)
	return BuildBoundFromLocal(local, out);
}


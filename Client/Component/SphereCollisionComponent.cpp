#include "pch.h"
#include "SphereCollisionComponent.h"
#include "AABBCollisionComponent.h"
#include "OBBCollisionComponent.h"
#include "Collision/CollisionSystem.h"

#include "Object/Asset/AssetManager.h"
#include "Object/Asset/Mesh/Mesh.h"

#include "../Editor/EditEngine.h"


bool SphereCollisionComponent::Init(int32 id, const std::string name, Ptr<IComponentOwner> owner)
{
	CollisionComponent::Init(id, name, owner);
	_shape = eCollisonShape::SPHERE;

#if _DEBUG
	_mesh = MESH_MANAGER->FindMesh("FrameSphere");
#endif
	_type = eComponentType::SPHERE;
	return true;
}

void SphereCollisionComponent::Tick(float deltaTime)
{
	CollisionComponent::Tick(deltaTime);
	_min._x = _world._position._x - _radius;
	_min._y = _world._position._y - _radius;

	_max._x = _world._position._x + _radius;
	_max._y = _world._position._y - _radius;
	SetWorldScale(_radius * 2.f, _radius * 2.f, 1.f);
}

void SphereCollisionComponent::Collision(float deltaTime)
{
	CollisionComponent::Collision(deltaTime);
}

void SphereCollisionComponent::Render(float deltaTime)
{
	CollisionComponent::Render(deltaTime);
}

void SphereCollisionComponent::DrawInspector()
{
	CollisionComponent::DrawInspector();

	ImGui::SeparatorText("SphereCollision");
	ImGui::DragFloat("SPHERE", &_radius);
}

void SphereCollisionComponent::Destroy()
{
	CollisionComponent::Destroy();
}

bool SphereCollisionComponent::Collision(Weak<CollisionComponent> dest)
{
	Ptr<CollisionComponent> destCol = Lock<CollisionComponent>(dest);
	if (nullptr == destCol)
	{
		return false;
	}

	switch (destCol->GetShape())
	{
	case eCollisonShape::SPHERE:
		return CollisionSystem::SphereToSphere(This<SphereCollisionComponent>(), Cast<CollisionComponent, SphereCollisionComponent>(destCol));
	case eCollisonShape::AABB:
		return CollisionSystem::AABBToSphere(Cast<CollisionComponent, AABBCollisionComponent>(destCol), This<SphereCollisionComponent>());
	case eCollisonShape::OBB:
		return CollisionSystem::OBBToSphere(Cast<CollisionComponent, OBBCollisionComponent>(destCol), This<SphereCollisionComponent>());
	}
	return false;
}

bool SphereCollisionComponent::Save(std::ofstream& file)
{
	return true;
}

bool SphereCollisionComponent::Load(std::ifstream& file)
{
	return true;
}

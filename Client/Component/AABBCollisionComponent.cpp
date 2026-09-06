#include "pch.h"
#include "AABBCollisionComponent.h"
#include "OBBCollisionComponent.h"
#include "SphereCollisionComponent.h"
#include "EdgeCollisionComponent.h"
#include "Object/Asset/AssetManager.h"
#include "Collision/CollisionSystem.h"

#include "../Editor/EditEngine.h"


FLocalBound2D AABBCollisionComponent::GetLocalBound()
{
	FLocalBound2D localBound;
	localBound._Min._x = _box._min._x;
	localBound._Min._y = _box._min._y;

	localBound._Max._x = _box._max._x;
	localBound._Max._y = _box._max._y;
	return localBound;
}

bool AABBCollisionComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	CollisionComponent::Init(id, name, owner);
	_shape = eCollisonShape::AABB;

#if _DEBUG
	_mesh = MESH_MANAGER->FindMesh("frameRect");
#endif
	_type = eComponentType::AABB;
	return true;
}

void AABBCollisionComponent::Tick(float deltaTime)
{
	CollisionComponent::Tick(deltaTime);
	_box._min._x = _world._position._x - _boxSize._x * 0.5f;
	_box._max._x = _world._position._x + _boxSize._x * 0.5f;

	_box._min._y = _world._position._y - _boxSize._y * 0.5f;
	_box._max._y = _world._position._y + _boxSize._y * 0.5f;

	_min._x = _box._min._x;
	_max._x = _box._max._x;

	_min._y = _box._min._y;
	_max._y = _box._max._y;
	SetWorldScale(_boxSize);
}

void AABBCollisionComponent::Collision(float deltaTime)
{
	CollisionComponent::Collision(deltaTime);
}

void AABBCollisionComponent::Render(float deltaTime)
{
	CollisionComponent::Render(deltaTime);
}

void AABBCollisionComponent::DrawInspector()
{
	CollisionComponent::DrawInspector();
	ImGui::SeparatorText("AABBCollision");

	ImGui::DragFloat2("AABB", &_boxSize._x);
}

void AABBCollisionComponent::Destroy()
{
	CollisionComponent::Destroy();
}

bool AABBCollisionComponent::Collision(Weak<CollisionComponent> dest)
{
	Ptr<CollisionComponent> destCol = Lock<CollisionComponent>(dest);
	if (nullptr == destCol)
	{
		return false;
	}

	switch (destCol->GetShape())
	{
	case eCollisonShape::AABB:
		return CollisionSystem::AABBToAABB(This<AABBCollisionComponent>(), Cast<CollisionComponent, AABBCollisionComponent>(destCol));
	case eCollisonShape::SPHERE:
		return CollisionSystem::AABBToSphere(This<AABBCollisionComponent>(), Cast<CollisionComponent, SphereCollisionComponent>(destCol));
	case eCollisonShape::OBB:
		return CollisionSystem::AABBToOBB(This<AABBCollisionComponent>(), Cast<CollisionComponent, OBBCollisionComponent>(destCol));
	case eCollisonShape::EDGE:
		return CollisionSystem::EdgeToAABB(Cast<CollisionComponent, EdgeCollisionComponent>(destCol),
			This<AABBCollisionComponent>());
	}
	return false;
}

bool AABBCollisionComponent::Save(std::ofstream& file)
{
	return true;
}

bool AABBCollisionComponent::Load(std::ifstream& file)
{
	return true;
}

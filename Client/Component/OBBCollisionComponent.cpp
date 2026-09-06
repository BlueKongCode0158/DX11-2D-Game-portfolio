#include "pch.h"
#include "OBBCollisionComponent.h"
#include "SphereCollisionComponent.h"
#include "AABBCollisionComponent.h"

#include "Object/Asset/AssetManager.h"
#include "Object/Asset/Mesh/MeshManager.h"
#include "Collision/CollisionSystem.h"

#include "../Editor/EditEngine.h"


bool OBBCollisionComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	_shape = eCollisonShape::OBB;
	CollisionComponent::Init(id, name, owner);
#if _DEBUG
	_mesh = MESH_MANAGER->FindMesh("frameRect");
#endif
	_type = eComponentType::OBB;
	return true;
}

void OBBCollisionComponent::Tick(float deltaTime)
{
	CollisionComponent::Tick(deltaTime);
	_box._center._x = _world._position._x;
	_box._center._y = _world._position._y;

	_box._axis[AXIS_TYPE::X]._x = _axis[AXIS_TYPE::X]._x;
	_box._axis[AXIS_TYPE::X]._y = _axis[AXIS_TYPE::X]._y;

	_box._axis[AXIS_TYPE::Y]._x = _axis[AXIS_TYPE::Y]._x;
	_box._axis[AXIS_TYPE::Y]._y = _axis[AXIS_TYPE::Y]._y;

	_box._halfSize = _boxSize * 0.5f;

	FVector2D pos[4];
	pos[0] = _box._center - _box._axis[AXIS_TYPE::X] * _box._halfSize._x + _box._axis[AXIS_TYPE::Y] * _box._halfSize._y;
	pos[1] = _box._center - _box._axis[AXIS_TYPE::X] * _box._halfSize._x - _box._axis[AXIS_TYPE::Y] * _box._halfSize._y;
	pos[2] = _box._center + _box._axis[AXIS_TYPE::X] * _box._halfSize._x + _box._axis[AXIS_TYPE::Y] * _box._halfSize._y;
	pos[3] = _box._center + _box._axis[AXIS_TYPE::X] * _box._halfSize._x - _box._axis[AXIS_TYPE::Y] * _box._halfSize._y;

	_min._x = pos[0]._x;
	_min._y = pos[0]._y;

	_min._x = pos[0]._x;
	_max._x = pos[0]._y;

	for (int i = 0; i < 4; i++)
	{
		_min._x = _min._x > pos[i]._x ? pos[i]._x : _min._x;
		_min._y = _min._y > pos[i]._y ? pos[i]._y : _min._y;
		_max._x = _max._x < pos[i]._x ? pos[i]._x : _max._x;
		_max._y = _max._y < pos[i]._y ? pos[i]._y : _max._y;
	}

	SetWorldScale(_boxSize);
}

void OBBCollisionComponent::Collision(float deltaTime)
{
	CollisionComponent::Collision(deltaTime);
}

void OBBCollisionComponent::Render(float deltaTime)
{
	CollisionComponent::Render(deltaTime);
}

void OBBCollisionComponent::DrawInspector()
{
	CollisionComponent::DrawInspector();
	ImGui::SeparatorText("OBBCollision");
	ImGui::DragFloat2("OBB", &_boxSize._x);
}

void OBBCollisionComponent::Destroy()
{
	CollisionComponent::Destroy();
}

bool OBBCollisionComponent::Save(std::ofstream& file)
{
	return true;
}

bool OBBCollisionComponent::Load(std::ifstream& file)
{
	return true;
}

bool OBBCollisionComponent::Collision(Weak<CollisionComponent> dest)
{
	Ptr<CollisionComponent> destCol = Lock<CollisionComponent>(dest);
	if (nullptr == destCol)
	{
		return false;
	}

	switch (destCol->GetShape())
	{
	case eCollisonShape::OBB:
		return CollisionSystem::OBBToOBB(This<OBBCollisionComponent>(), Cast<CollisionComponent, OBBCollisionComponent>(destCol));
	case eCollisonShape::AABB:
		return CollisionSystem::AABBToOBB(Cast<CollisionComponent, AABBCollisionComponent>(destCol), This<OBBCollisionComponent>());
	case eCollisonShape::SPHERE:
		return CollisionSystem::OBBToSphere(This<OBBCollisionComponent>(), Cast<CollisionComponent, SphereCollisionComponent>(destCol));
	}
	return false;
}

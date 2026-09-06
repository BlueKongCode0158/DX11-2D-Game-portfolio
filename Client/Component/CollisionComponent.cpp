#include "pch.h"
#include "CollisionComponent.h"
#include "Collision/CollisionProfileManager.h"
#include "Shader/ShaderManager.h"
#include "Shader/ConstantBuffer/TransformCBuffer.h"
#include "Shader/ConstantBuffer/ColorCBuffer.h"
#include "Shader/Shader.h"
#include "World/Level.h"
#include "Object/Asset/Mesh/Mesh.h"

#include "../Editor/EditEngine.h"

CollisionComponent::CollisionComponent()
{
#ifdef _DEBUG
	_isRender = true;
	_renderLayerName = "Collider";
#endif // _DEBUG

}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	SceneComponent::Init(id, name, owner);
	
	_colliderID = std::make_pair<int32, int32>(owner->GetOwnerID(), GetComponentID());

	Ptr<Level> level = owner->GetLevel();
	if (nullptr == level)
	{
		return false;
	}
	
	level->AddCollision(_colliderID, This<CollisionComponent>());
#if _DEBUG
	_shader = ShaderManager::Instance().FindShader("FrameMeshShader");
	_transformCBuffer = ShaderManager::Instance().FindCBuffer<TransformCBuffer>("Transform");
	_colorCBuffer = ShaderManager::Instance().FindCBuffer<ColorCBuffer>("Color");
#endif
	_type = eComponentType::COLLISION;
	return true;
}

void CollisionComponent::Tick(float deltaTime)
{
	SceneComponent::Tick(deltaTime);
}

void CollisionComponent::Collision(float deltaTime)
{
	SceneComponent::Collision(deltaTime);
}

void CollisionComponent::Render(float deltaTime)
{
	SceneComponent::Render(deltaTime);
#if _DEBUG
	if (!_mesh || !_shader)
	{
		return;
	}

	_transformCBuffer->SetWorldMatrix(_matrix._world);
	Ptr<Level> level = Lock<Level>(_level);

	if (nullptr == level)
	{
		return;
	}

	_transformCBuffer->SetViewMatrix(level->GetViewMatrix());
	_transformCBuffer->SetProjMatrix(level->GetProjMatrix());
	_transformCBuffer->Update();

	if (_colliderReations.size() > 0)
	{
		_colorCBuffer->SetColor(1.f, 0.f, 0.f, 1.f);
	}
	else
	{
		_colorCBuffer->SetColor(0.f, 1.f, 0.f, 1.f);
	}
	_colorCBuffer->Update();
	_shader->SetShader();
	_mesh->Render();
#endif

}

void CollisionComponent::DrawInspector()
{
	SceneComponent::DrawInspector();

	ImGui::SeparatorText("Collision");
}

void CollisionComponent::Destroy()
{
	SceneComponent::Destroy();
	Ptr<Level> level = Lock<Level>(_level);

	if (nullptr == level)
	{
		return;
	}
	level->RemoveCollision(_colliderID);

	for (auto& it : _colliderReations)
	{
		std::pair<int32, int32> key = it.first;
		Ptr<CollisionComponent> foundCol = level->FindCollider(key);
		if (nullptr == foundCol)
		{
			continue;
		}
		foundCol->_colliderReations.erase(_colliderID);
	}
}

bool CollisionComponent::Save(std::ofstream& file)
{
	return true;
}

bool CollisionComponent::Load(std::ifstream& file)
{
	return true;
}

bool CollisionComponent::Collision(Weak<CollisionComponent> dest)
{
	// 충돌시 logic 추가
	return true;
}

void CollisionComponent::SetCollisionProfile(const std::string& name)
{
	Ptr<CollisionProfile> foundVal = CollisionProfileManager::Instance().FindProfile(name);
	if (nullptr == foundVal)
	{
		return;
	}

	_profile = foundVal;
}

const Ptr<class CollisionProfile> CollisionComponent::GetProfile() const
{
	return _profile;
}

eCollisionState CollisionComponent::CheckState(std::pair<int32, int32>& destKey)
{
	auto it = _colliderReations.find(destKey);
	if (_colliderReations.end() == it)
	{
		return eCollisionState::COLLISION_STATE_RELEASE;
	}
	return it->second;
}

const std::pair<int32, int32>& CollisionComponent::GetColliderID() const
{
	return _colliderID;
}

void CollisionComponent::Invoke(eCollisionState state, Weak<CollisionComponent> dest, const std::pair<int32, int32>& destKey)
{
	Ptr<CollisionComponent> destCol = Lock<CollisionComponent>(dest);
	if (nullptr == destCol)
	{
		return;
	}

	if (state == eCollisionState::COLLISION_STATE_BLOCK ||
		state == eCollisionState::COLLISION_STATE_OVERLAP)
	{
		_colliderReations.emplace(std::make_pair(destKey, state));
	}
	else
	{
		_colliderReations.erase(destKey);
	}

	if (_collisionCallBack[state])
	{
		_collisionCallBack[state](dest);
	}
}

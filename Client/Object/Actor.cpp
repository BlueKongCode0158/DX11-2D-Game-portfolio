#include "pch.h"
#include "Actor.h"
#include "World/Level.h"
#include "Component/SceneComponent.h"
#include "Component/ActorComponent.h"
#include "../Component/StaticMeshComponent.h"
#include "../Component/MeshComponent.h"

#include "../Editor/EditEngine.h"

#include <filesystem>
#include <fstream>

Actor::Actor()
{
}

Actor::~Actor()
{
}

bool Actor::Init(int32 id, const FVector3D& pos, const FVector3D& scale, const FRotator& rot, const std::string& name)
{
	_id = id;
	_root = CreateSceneComponent<SceneComponent>("DefaultRoot");

	SetRelativePosition(pos);
	SetRelativeScale(scale);
	SetRelativeRotation(rot);
	SetName(name);

	SetEnable(true);
	SetActive(true);
	_type = eActorType::Actor;

	return true;
}

void Actor::Tick(float deltaTime)
{
	for (auto& it : _actorComponents)
	{
		Ptr<ActorComponent> comp = it.second;
		if (nullptr == comp)
		{
			continue;
		}
		if (false == comp->IsActive() || false == comp->IsEnable())
		{
			continue;
		}
		comp->Tick(deltaTime);
	}
	_root->Tick(deltaTime);
}

void Actor::Collision(float deltaTime)
{
}

void Actor::Render(float deltaTime)
{
	//_root->Render(deltaTime);
}

void Actor::DrawInspector()
{
	ImGui::SeparatorText("Actor");
	ImGui::SeparatorText("World Transform");

	FVector3D pos = GetWorldPosition();
	if (ImGui::DragFloat3("World Position", &pos._x, 0.1f))
	{
		SetWorldPosition(pos);
	}
	FVector3D rot = GetWorldRotation();
	if (ImGui::DragFloat3("World Rotation", &rot._x, 0.1f))
	{
		SetWorldRotation(rot);
	}
	FVector3D scale = GetWorldScale();
	if (ImGui::DragFloat3("World Scale", &scale._x, 0.1f))
	{
		SetWorldScale(scale);
	}

	ImGui::BeginGroup();

	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active))
	{
		SetActive(active);
	}

	ImGui::SameLine();

	bool enable = IsEnable();
	if (ImGui::Checkbox("Enble", &enable))
	{
		SetEnable(enable);
	}

	ImGui::EndGroup();

	const char* buf = _name.c_str();
	if (ImGui::InputText("ActorName", const_cast<char*>(buf), 128))
	{
		SetName(buf);
	}

	_root->DrawInspector();

	for (auto& [index, actorComp] : _actorComponents)
	{
		actorComp->DrawInspector();
	}
}

void Actor::SetEnable(bool enable)
{
	Object::SetEnable(enable);
	_root->SetEnable(enable);
}

Ptr<class SceneComponent> Actor:: GetRoot() const
{
	return _root;
}

void Actor::SetRootComponent(const Ptr<class SceneComponent> comp)
{
	if (nullptr != _root)
	{
		comp->SetRelativeTransform(_root->GetRelativeTransform());
		
		for (auto& it : _root->_childs)
		{
			Ptr<SceneComponent> child = it.second;
			if (nullptr != child)
			{
				child->AttachToComponent(comp);
			}
		}
		_root->_childs.clear();
		Delete(_root);
		_root = comp;
	}
	else
	{
		_root = comp;
	}
}

Ptr<class SceneComponent> Actor::FindSceneComponent(const std::string& name) const
{
	if (_root->GetName() == name)
	{
		return _root;
	}

	auto it = _componentFinder.find(name);
	if (_componentFinder.end() == it)
	{
		return nullptr;
	}
	return FindSceneComponent(it->second);
}

Ptr<class ActorComponent> Actor::FindActorComponent(const std::string& name) const
{
	auto it = _componentFinder.find(name);
	if (_componentFinder.end() == it)
	{
		return nullptr;
	}
	return FindActorComponent(it->second);
}

Ptr<class SceneComponent> Actor::FindSceneComponent(int32 id) const
{
	if (_root->GetComponentID() == id)
	{
		return _root;
	}
	return _root->FindComponent(id);
}

Ptr<class ActorComponent> Actor::FindActorComponent(int32 id) const
{
	auto compIt = _actorComponents.find(id);
	if (_actorComponents.end() == compIt)
	{
		return nullptr;
	}
	return compIt->second;
}

Ptr<class Level> Actor::GetLevel()
{
	return Lock<Level>(_level);
}

int32 Actor::GetOwnerID()
{
	return _id;
}

void Actor::SetLevel(Ptr<class Level> level)
{
	_level = level;
}

void Actor::SetName(const std::string& name)
{
	_name = name;
}

void Actor::Remove()
{
	Ptr<Level> level = Lock<Level>(_level);
	if (level != nullptr)
	{
		level->RemoveActor(_id);
	}
}

const std::string& Actor::GetName() const
{
	return _name;
}

bool Actor::IsTag(const std::string& tag)
{
	return _tags.contains(tag);
}

void Actor::AddTag(const std::string& tag)
{
	_tags.insert(tag);
	Ptr<Level> level = GetLevel();
	if (nullptr == level)
	{
		return;
	}
	level->AddTag(tag, _id);
}

eActorType Actor::GetType() const
{
	return _type;
}

const FVector3D& Actor::GetWorldScale() const
{
	return _root->GetWorldScale();
}

const FVector3D& Actor::GetWorldRotation() const
{
	return _root->GetWorldRotation();
}

const FVector3D& Actor::GetWorldPosition() const
{
	return _root->GetWorldPosition();
}

const FTransform& Actor::GetWorldTransform() const
{
	return _root->GetWorldTransform();
}

void Actor::SetWorldScale(const FVector3D& scale)
{
	_root->SetWorldScale(scale);
}

void Actor::SetWorldScale(const float x, const float y, const float z)
{
	_root->SetWorldScale(x, y, z);
}

void Actor::SetWorldScale(const FVector2D& scale)
{
	_root->SetWorldScale(scale);
}

void Actor::SetWorldScale(const float x, const float y)
{
	_root->SetWorldScale(x, y);
}

void Actor::SetWorldRotation(const FRotator& rot)
{
	_root->SetWorldRotation(rot);
}

void Actor::SetWorldRotation(const FVector2D& rot)
{
	_root->SetWorldRotation(rot);
}

void Actor::SetWorldRotation(const float x, const float y, const float z)
{
	_root->SetWorldRotation(x, y, z);
}

void Actor::SetWorldRotation(const float x, const float y)
{
	_root->SetWorldRotation(x, y);
}

const FVector3D& Actor::AddWorldScale(const FVector3D& scale)
{
	return _root->_world._scale;
}

const FVector3D& Actor::AddWorldScale(const FVector2D& scale)
{
	return _root->AddWorldScale(scale);
}

const FVector3D& Actor::AddWorldScale(const float x, const float y, const float z)
{
	return _root->AddWorldScale(x, y, z);
}

const FVector3D& Actor::AddWorldScale(const float x, const float y)
{
	return _root->AddWorldScale(x, y);
}

const FVector3D& Actor::AddWorldScale(const float value)
{
	return _root->AddWorldScale(value);
}

const FVector3D& Actor::AddWorldScaleX(float x)
{
	return _root->AddWorldScaleX(x);
}

const FVector3D& Actor::AddWorldScaleY(float y)
{
	return _root->AddWorldScaleY(y);
}

const FVector3D& Actor::AddWorldPosition(const FVector3D& pos)
{
	return _root->AddWorldPosition(pos);
}

const FVector3D& Actor::AddWorldPosition(const FVector2D& pos)
{
	return _root->AddWorldPosition(pos);
}

const FVector3D& Actor::AddWorldPosition(const float x, const float y)
{
	return _root->AddWorldPosition(x, y);
}

const FVector3D& Actor::AddWorldPosition(const float x, const float y, const float z)
{
	return _root->AddWorldPosition(x, y, z);
}

const FVector3D& Actor::AddWorldPosition(const float value)
{
	return _root->AddWorldPosition(value);
}

const FVector3D& Actor::AddWorldPositionX(const float x)
{
	return _root->AddWorldPositionX(x);
}

const FVector3D& Actor::AddWorldPositionY(const float y)
{
	return _root->AddWorldPositionY(y);
}

void Actor::SetWorldPosition(const FVector3D& pos)
{
	_root->SetWorldPosition(pos);
}

void Actor::SetWorldPosition(const FVector2D& pos)
{
	_root->SetWorldPosition(pos);
}

void Actor::SetWorldPosition(float x, float y)
{
	_root->SetWorldPosition(x, y);
}

void Actor::SetWorldPosition(float x, float y, float z)
{
	_root->SetWorldPosition(x, y, z);
}

void Actor::SetWorldTransform(const FTransform& tran)
{
	_root->SetWorldTransform(tran);	
}

void Actor::SetWorldTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot)
{
	_root->SetWorldTransform(pos, scale, rot);
}

const FVector3D& Actor::GetRelativeScale() const
{
	return _root->GetRelativeScale();
}

const FVector3D& Actor::GetRelativeRotation() const
{
	return _root->GetRelativeRotator();
}

const FVector3D& Actor::GetRelativePosition() const
{
	return _root->GetRelativePosition();
}

const FTransform& Actor::GetRelativeTransform() const
{
	return _root->GetRelativeTransform();
}

void Actor::SetRelativeScale(const FVector3D& scale)
{
	_root->SetRelativeScale(scale);
}

void Actor::SetRelativeScale(const FVector2D& scale)
{
	_root->SetRelativeScale(scale);
}

void Actor::SetRelativeScale(const float x, const float y, const float z)
{
	_root->SetRelativeScale(x, y, z);
}

void Actor::SetRelativeScale(const float x, const float y)
{
	_root->SetRelativeScale(x, y);
}

void Actor::SetRelativeRotation(const FRotator& rot)
{
	_root->SetRelativeRotation(rot);
}

void Actor::SetRelativeRotation(const FVector2D& rot)
{
	_root->SetRelativeRotation(rot);
}

void Actor::SetRelativeRotation(const float x, const float y, const float z)
{
	_root->SetRelativeRotation(x, y, z);
}

void Actor::SetRelativeRotation(const float x, const float y)
{
	_root->SetRelativeRotation(x, y);
}

void Actor::SetRelativePosition(const FVector3D& pos)
{
	_root->SetRelativePosition(pos);
}

void Actor::SetRelativePosition(const FVector2D& pos)
{
	_root->SetRelativePosition(pos);
}

void Actor::SetRelativePosition(const float x, const float y, const float z)
{
	_root->SetRelativePosition(x, y, z);
}

void Actor::SetRelativePosition(const float x, const float y)
{
	_root->SetRelativePosition(x, y);
}

const FVector3D& Actor::AddRelativeScale(const FVector3D& scale)
{
	return _root->AddRelativeScale(scale);
}

const FVector3D& Actor::AddRelativeScale(const FVector2D& scale)
{
	return _root->AddRelativeScale(scale);
}

const FVector3D& Actor::AddRelativeScale(const float x, const float y, const float z)
{
	return _root->AddRelativeScale(x, y, z);
}

const FVector3D& Actor::AddRelativeScale(const float x, const float y)
{
	return _root->AddRelativeScale(x, y);
}

const FVector3D& Actor::AddRelativeScale(const float value)
{
	return _root->AddRelativeScale(value);
}

const FVector3D& Actor::AddRelativeScaleX(const float x)
{
	return _root->AddRelativeScaleX(x);
}

const FVector3D& Actor::AddRelativeScaleY(const float y)
{
	return _root->AddRelativeScaleY(y);
}

const FVector3D& Actor::AddRelativePosition(const FVector3D& pos)
{
	return _root->AddRelativePosition(pos);
}

const FVector3D& Actor::AddRelativePosition(const FVector2D& pos)
{
	return _root->AddRelativePosition(pos);
}

const FVector3D& Actor::AddRelativePosition(const float x, const float y)
{
	return _root->AddRelativePosition(x, y);
}

const FVector3D& Actor::AddRelativePosition(const float x, const float y, const float z)
{
	return _root->AddRelativePosition(x, y, z);
}

const FVector3D& Actor::AddRelativePosition(const float value)
{
	return _root->AddRelativePosition(value);
}

const FVector3D& Actor::AddRelativePositionX(const float x)
{
	return _root->AddRelativePositionX(x);
}

const FVector3D& Actor::AddRelativePositionY(const float y)
{
	return _root->AddRelativePositionY(y);
}

const FVector3D& Actor::AddRelativeRotation(const FRotator& rot)
{
	return _root->AddRelativeRotation(rot);
}

const FVector3D& Actor::AddRelativeRotation(const FVector2D& rot)
{
	return _root->AddRelativeRotation(rot);
}

const FVector3D& Actor::AddRelativeRotation(const float x, const float y)
{
	return _root->AddRelativeRotation(x, y);
}

const FVector3D& Actor::AddRelativeRotation(const float x, const float y, const float z)
{
	return _root->AddRelativeRotation(x, y, z);
}

const FVector3D& Actor::AddRelativeRotation(const float value)
{
	return _root->AddRelativeRotation(value);
}

const FVector3D& Actor::AddRelativeRotationX(const float x)
{
	return _root->AddRelativeRotationX(x);
}

const FVector3D& Actor::AddRelativeRotationY(const float y)
{
	return _root->AddRelativeRotationY(y);
}

void Actor::SetRelativeTransform(const FTransform& tran)
{
	_root->SetRelativeTransform(tran);
}

void Actor::SetRelativeTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot)
{
	_root->SetRelativeTransform(pos, scale, rot);
}

void Actor::Destroy()
{
	DESTROY(_root);

	for (auto& it : _actorComponents)
	{
		DESTROY(it.second);
	}
	_actorComponents.clear();
	_componentFinder.clear();
}

bool Actor::Save(std::ofstream& file)
{
	file.write(reinterpret_cast<const char*>(&_type), sizeof(eActorType));
	int32 nameStr = _name.length();
	file.write(reinterpret_cast<const char*>(&nameStr), sizeof(int32));
	file.write(_name.c_str(), nameStr);

	size_t tagSize = _tags.size();
	file.write(reinterpret_cast<const char*>(&tagSize), sizeof(size_t));

	for (auto& it : _tags)
	{
		int32 tagLen = it.length();
		file.write(reinterpret_cast<const char*>(&tagLen), sizeof(int32));
		file.write(it.c_str(), tagLen);
	}

	file.write(reinterpret_cast<const char*>(_root->GetComponentType()), sizeof(eComponentType));

	std::string rootName = _root->GetName();
	nameStr = rootName.length();

	file.write(reinterpret_cast<const char*>(&nameStr), sizeof(int32));
	file.write(_name.c_str(), nameStr);
	return true;
}

bool Actor::Load(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&_tags), sizeof(eActorType));
	int32 nameLen = 0;
	file.read(reinterpret_cast<char*>(&nameLen), sizeof(int32));
	_name.resize(nameLen);
	file.read(_name.data(), nameLen);
	
	size_t tagSize = 0;
	file.read(reinterpret_cast<char*>(&tagSize), sizeof(size_t));

	for (size_t i = 0; i < tagSize; i++)
	{
		int32 tagLen = 0;
		file.read(reinterpret_cast<char*>(&tagLen), sizeof(int32));
		std::string tag;
		tag.resize(nameLen);
		file.read(tag.data(), nameLen);

		_tags.insert(tag);
	}

	eComponentType readType = eComponentType::END;
	file.read(reinterpret_cast<char*>(&readType), sizeof(eComponentType));
	if (readType == eComponentType::END)
	{
		return false;
	}

	nameLen = 0;
	std::string rootName = "";
	file.read(reinterpret_cast<char*>(&nameLen), sizeof(int32));
	rootName.resize(nameLen);
	file.read(rootName.data(), nameLen);

	switch (readType)
	{
	case eComponentType::SCENE:
		_root = CreateSceneComponent<SceneComponent>(rootName);
		break;
	case eComponentType::STATICMESH:
		_root = CreateSceneComponent<StaticMeshComponent>(rootName);
		break;
	case eComponentType::MESH:
		_root = CreateSceneComponent<MeshComponent>(rootName);
		break;
	case eComponentType::COLLISION:
		break;
	case eComponentType::AABB:
		break;
	case eComponentType::OBB:
		break;
	case eComponentType::SPHERE:
		break;
	case eComponentType::SPRITE:
		break;
	case eComponentType::CAMERA:
		break;
	case eComponentType::SOUND:
		break;
	case eComponentType::TILE:
		break;
	case eComponentType::WIDGET:
		break;
	case eComponentType::FOGPLANE:
		break;
	case eComponentType::PARALLAX:
		break;
	case eComponentType::ACTOR:
		break;
	case eComponentType::MOVEMENT:
		break;
	case eComponentType::AI:
		break;
	case eComponentType::INPUT:
		break;
	case eComponentType::END:
		break;
	default:
		break;
	}

	_root->Load(file);

	for (auto& it : _actorComponents)
	{
		if (!it.second->IsActive())
		{
			continue;
		}
		it.second->Load(file);
	}
	return true;
}

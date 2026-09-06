#include "pch.h"
#include "SceneComponent.h"
#include "../Object/Actor.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"
#include "../Render/RenderManager.h"

#include "../Editor/EditEngine.h"


SceneComponent::SceneComponent() : _renderLayerName("Default"), _isRender(false)
{
	_eBlendState = eBlendMode::DEFAULT;
}

SceneComponent::~SceneComponent()
{
}

const Ptr<SceneComponent> SceneComponent::GetParent() const
{
	return Lock<SceneComponent>(_parent);
}


// 언리얼을 따라 만들었다.
void SceneComponent::AttachToComponent(Ptr<SceneComponent> comp)
{
	if (comp->GetComponentID() == GetComponentID())
	{
		return;
	}

	comp->AddChild(This<SceneComponent>());
	UpdateTransform();
}

void SceneComponent::AddChild(Ptr<SceneComponent> comp)
{
	if (nullptr == comp)
	{
		return;
	}

	if (comp->GetComponentID() == GetComponentID())
	{
		return;
	}

	comp->_parent = This<SceneComponent>();
	_childs[comp->_id] = comp;

	_transformDirty = true;
}

Ptr<SceneComponent> SceneComponent::FindComponent(int32 id)
{
	auto it = _childs.find(id);
	if (_childs.end() == it)
	{
		for (auto& subIt : _childs)
		{
			Ptr<SceneComponent> comp = subIt.second->FindComponent(id);
			if (nullptr != comp)
			{
				return comp;
			}
		}
		return nullptr;
	}
	else
	{
		return it->second;
	}
}

bool SceneComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	Component::Init(id, name, owner);

	_transformCBuffer = ShaderManager::Instance().FindCBuffer<TransformCBuffer>("Transform");

	if (_isRender)
	{
		RenderManager::Instance().AddRenderComponent(owner->GetOwnerID(), This<SceneComponent>());
	}
	_type = eComponentType::SCENE;

	UpdateTransform();
	return true;
}

void SceneComponent::Tick(float deltaTime)
{
	Component::Tick(deltaTime);

	if (_transformDirty)
	{
		UpdateTransform();
		_transformDirty = false;
	}

	for (auto& it : _childs)
	{
		Ptr<SceneComponent> child = it.second;
		if (nullptr == child)
		{
			continue;
		}
		if (false == child->IsActive() || false == child->IsEnable())
		{
			continue;
		}
		child->Tick(deltaTime);
	}
}

void SceneComponent::Collision(float deltaTime)
{
	Component::Collision(deltaTime);
}

void SceneComponent::Render(float deltaTime)
{
	if (_transformDirty)
	{
		UpdateTransform();
		_transformDirty = false;
	}
	Component::Render(deltaTime);
}

void SceneComponent::DrawInspector()
{
	Component::DrawInspector();
	ImGui::PushID(GetComponentID());

	ImGui::SeparatorText("SceneComponent");
	ImGui::SeparatorText("World Transform");
	FVector3D worldPos = GetWorldPosition();

	if (ImGui::DragFloat3("World Transform", &worldPos._x, 0.5f))
	{
		SetWorldPosition(worldPos);
		UpdateTransform();
	}

	FRotator worldRot = GetWorldRotation();
	if (ImGui::DragFloat3("World Rotation", &worldRot._x, 0.5f))
	{
		SetWorldRotation(worldRot);
		UpdateTransform();
	}

	FVector3D worldScale = GetWorldScale();
	if (ImGui::DragFloat3("World Scale", &worldScale._x, 0.5f))
	{
		SetWorldScale(worldScale);
		UpdateTransform();
	}

	ImGui::SeparatorText("Relative Transform");

	FVector3D relativePos = GetRelativePosition();
	if (ImGui::DragFloat3("Relative Position", &relativePos._x, 0.5f))
	{
		SetRelativePosition(relativePos);
		UpdateTransform();
	}

	FRotator relativeRot = GetRelativeRotator();
	if (ImGui::DragFloat3("Relative Rotation", &relativeRot._x, 0.5f))
	{
		SetRelativeRotation(relativeRot);
		UpdateTransform();
	}

	FVector3D relativeScale = GetRelativeScale();
	if (ImGui::DragFloat3("Relative Scale", &relativeScale._x, 0.5f))
	{
		SetRelativeScale(relativeScale);
		UpdateTransform();
	}
	ImGui::PopID();
}

void SceneComponent::Destroy()
{
	if (_isRender) {

		if (Ptr<IComponentOwner> owner = GetOwner())
		{
			RenderManager::Instance().RemoveRenderComponent(GetRenderLayerName(), owner->GetOwnerID(), GetComponentID());
		}
	}

	for (auto& it : _childs)
	{
		DESTROY(it.second);
	}
	_childs.clear();
}

bool SceneComponent::Save(std::ofstream& file)
{
	return false;
}

bool SceneComponent::Load(std::ifstream& file)
{
	return false;
}

void SceneComponent::SetEnable(bool enable)
{
	Object::SetEnable(enable);
	if (!_childs.empty())
	{
		for (auto& it : _childs)
		{
			it.second->SetEnable(enable);
		}
	}
}

const std::map<int32, Ptr<SceneComponent>>& SceneComponent::GetChilds() const
{
	return _childs;
}

const FTransform& SceneComponent::GetWorldTransform() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return _world;
}

void SceneComponent::SetWorldTransform(const FTransform& transform)
{
	SetWorldTransform(transform._position, transform._scale, transform._rotation);
}

void SceneComponent::SetWorldTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot)
{
	SetWorldScale(scale);
	SetWorldRotation(rot);
	SetWorldPosition(pos);
}

const FVector3D& SceneComponent::GetWorldScale() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return _world._scale;
}

void SceneComponent::SetWorldScale(const FVector3D& scale)
{
	SetWorldScale(scale._x, scale._y, scale._z);
}

void SceneComponent::SetWorldScale(float x, float y, float z)
{
	// TODO : SetWorldScale 구현
	if (Ptr<SceneComponent> parentComp = Lock<SceneComponent>(_parent))
	{
		_relative._scale._x = x / parentComp->_world._scale._x;
		_relative._scale._y = y / parentComp->_world._scale._y;
		_relative._scale._z = z / parentComp->_world._scale._z;
	}
	else
	{
		_relative._scale._x = x;
		_relative._scale._y = y;
		_relative._scale._z = z;
	}
	_transformDirty = true;
}

void SceneComponent::SetWorldScale(const FVector2D& scale)
{
	SetWorldScale(scale._x, scale._y, _world._scale._z);
}

void SceneComponent::SetWorldScale(float x, float y)
{
	SetWorldScale(x, y, _world._scale._z);
}

const FVector3D& SceneComponent::AddWorldScale(const FVector3D& scale)
{
	SetWorldScale(scale._x + _world._scale._x, scale._y + _world._scale._y, scale._z + _world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::AddWorldScale(const FVector2D& scale)
{
	SetWorldScale(scale._x + _world._scale._x, scale._y + _world._scale._y,_world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::AddWorldScale(const float x, const float y, const float z)
{
	SetWorldScale(x + _world._scale._x, y + _world._scale._y, z + _world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::AddWorldScale(const float x, const float y)
{
	SetWorldScale(x + _world._scale._x, y + _world._scale._y, _world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::AddWorldScale(const float value)
{
	SetWorldScale(value + _world._scale._x, value + _world._scale._y, _world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::AddWorldScaleX(const float x)
{
	SetWorldScale(x + _world._scale._x, _world._scale._y, _world._scale._z);
	return _world._scale;

}

const FVector3D& SceneComponent::AddWorldScaleY(const float y)
{
	SetWorldScale(_world._scale._x, y + _world._scale._y, _world._scale._z);
	return _world._scale;
}

const FVector3D& SceneComponent::GetWorldPosition() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return _world._position;
}

void SceneComponent::SetWorldPosition(const FVector3D& pos)
{
	SetWorldPosition(pos._x, pos._y, pos._z);
}

void SceneComponent::SetWorldPosition(float x, float y, float z)
{
	// TODO SetWorldPosition 구현
	if (Ptr<SceneComponent> parentComp = Lock<SceneComponent>(_parent))
	{
		FMatrix matParentInv = parentComp->_matrix._world;
		matParentInv.Inverse();
		_relative._position = FVector3D(x, y, z).TransformCoord(matParentInv);
	}
	else
	{
		_relative._position._x = x;
		_relative._position._y = y;
		_relative._position._z = z;
	}

	_transformDirty = true;
}

void SceneComponent::SetWorldPosition(const FVector2D& pos)
{
	SetWorldPosition(pos._x, pos._y, _world._position._z);
}

void SceneComponent::SetWorldPosition(float x, float y)
{
	SetWorldPosition(x, y, _world._position._z);
}

const FVector3D& SceneComponent::AddWorldPosition(const FVector3D& pos)
{
	SetWorldPosition(pos._x + _world._position._x, pos._y + _world._position._y, pos._z + _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPosition(const FVector2D& pos)
{
	SetWorldPosition(pos._x + _world._position._x, pos._y + _world._position._y, _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPosition(const float x, const float y, const float z)
{
	SetWorldPosition(x + _world._position._x, y + _world._position._y, z + _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPosition(const float x, const float y)
{
	SetWorldPosition(x + _world._position._x, y + _world._position._y, _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPosition(const float value)
{
	SetWorldPosition(value + _world._position._x, value + _world._position._y, _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPositionX(const float x)
{
	SetWorldPosition(x + _world._position._x, _world._position._y, _world._position._z);
	return _world._position;
}

const FVector3D& SceneComponent::AddWorldPositionY(const float y)
{
	SetWorldPosition(_world._position._x, y + _world._position._y, _world._position._z);
	return _world._position;
}

const FRotator& SceneComponent::GetWorldRotation() const
{
	return _world._rotation;
}

void SceneComponent::SetWorldRotation(const FRotator& rot)
{
	SetWorldRotation(rot._x, rot._y, rot._z);
}

void SceneComponent::SetWorldRotation(float x, float y, float z)
{
	if (Ptr<SceneComponent> parentComp = Lock<SceneComponent>(_parent))
	{
		_relative._rotation._x = x - parentComp->_world._rotation._x;
		_relative._rotation._y = y - parentComp->_world._rotation._y;
		_relative._rotation._z = z - parentComp->_world._rotation._z;
	}
	else
	{
		_relative._rotation._x = x;
		_relative._rotation._y = y;
		_relative._rotation._z = z;
	}

	_transformDirty = true;
}

void SceneComponent::SetWorldRotation(const FVector2D& rot)
{
	SetWorldRotation(rot._x, rot._y, _world._rotation._z);
}

void SceneComponent::SetWorldRotation(float x, float y)
{
	SetWorldRotation(x, y, _world._rotation._z);
}

const FTransform& SceneComponent::GetRelativeTransform() const
{
	return _relative;
}

void SceneComponent::SetRelativeTransform(const FTransform& transform)
{
	SetRelativeTransform(transform._position, transform._scale, transform._rotation);
}

void SceneComponent::SetRelativeTransform(const FVector3D& pos, const FVector3D& scale, const FRotator& rot)
{
	SetRelativeScale(scale);
	SetRelativeRotation(rot);
	SetRelativePosition(pos);
}

const FVector3D& SceneComponent::GetRelativeScale() const
{
	return _relative._scale;
}

void SceneComponent::SetRelativeScale(const FVector3D& scale)
{
	SetRelativeScale(scale._x, scale._y, scale._z);
}

void SceneComponent::SetRelativeScale(float x, float y, float z)
{
	// TODO SetRelativeScale
	_relative._scale._x = x;
	_relative._scale._y = y;
	_relative._scale._z = z;

	_transformDirty = true;
}

void SceneComponent::SetRelativeScale(const FVector2D& scale)
{
	SetRelativeScale(scale._x, scale._y, _relative._scale._z);
}

void SceneComponent::SetRelativeScale(float x, float y)
{
	SetRelativeScale(x, y, _relative._scale._z);
}

const FVector3D& SceneComponent::AddRelativeScale(const FVector3D& scale)
{
	SetRelativeScale(scale._x + _relative._scale._x, scale._y + _relative._scale._y, scale._z + _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScale(const FVector2D& scale)
{
	SetRelativeScale(scale._x + _relative._scale._x, scale._y + _relative._scale._y, _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScale(const float x, const float y, const float z)
{
	SetRelativeScale(x + _relative._scale._x, y + _relative._scale._y, z + _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScale(const float x, const float y)
{
	SetRelativeScale(x + _relative._scale._x, y + _relative._scale._y, _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScale(const float value)
{
	SetRelativeScale(value + _relative._scale._x, value + _relative._scale._y, _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScaleX(const float x)
{
	SetRelativeScale(x + _relative._scale._x, _relative._scale._y, _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::AddRelativeScaleY(const float y)
{
	SetRelativeScale(_relative._scale._x, y + _relative._scale._y, _relative._scale._z);
	return _relative._scale;
}

const FVector3D& SceneComponent::GetRelativePosition() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return _relative._position;
}

void SceneComponent::SetRelativePosition(const FVector3D& pos)
{
	SetRelativePosition(pos._x, pos._y, pos._z);
}

void SceneComponent::SetRelativePosition(float x, float y, float z)
{
	// TODO : SetRelativePosition
	_relative._position._x = x;
	_relative._position._y = y;
	_relative._position._z = z;

	_transformDirty = true;
}

void SceneComponent::SetRelativePosition(const FVector2D& pos)
{
	SetRelativePosition(pos._x, pos._y, _relative._position._z);
}

void SceneComponent::SetRelativePosition(float x, float y)
{
	SetRelativePosition(x, y, _relative._position._z);
}

const FVector3D& SceneComponent::AddRelativePosition(const FVector3D& pos)
{
	SetRelativePosition(pos._x + _relative._position._x, pos._y + _relative._position._y, pos._z + _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePosition(const FVector2D& pos)
{
	SetRelativePosition(pos._x + _relative._position._x, pos._y + _relative._position._y, _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePosition(const float x, const float y, const float z)
{
	SetRelativePosition(x + _relative._position._x, y + _relative._position._y, z + _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePosition(const float x, const float y)
{
	SetRelativePosition(x + _relative._position._x, y + _relative._position._y, _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePosition(const float value)
{
	SetRelativePosition(value + _relative._position._x, value + _relative._position._y, _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePositionX(const float x)
{
	SetRelativePosition(x + _relative._position._x, _relative._position._y, _relative._position._z);
	return _relative._position;
}

const FVector3D& SceneComponent::AddRelativePositionY(const float y)
{
	SetRelativePosition(_relative._position._x, y + _relative._position._y, _relative._position._z);
	return _relative._position;
}

const FRotator& SceneComponent::GetRelativeRotator() const
{
	return _relative._rotation;
}

void SceneComponent::SetRelativeRotation(const FRotator& rot)
{
	SetRelativeRotation(rot._x, rot._y, rot._z);
}

void SceneComponent::SetRelativeRotation(float x, float y, float z)
{
	_relative._rotation._x = x;
	_relative._rotation._y = y;
	_relative._rotation._z = z;

	_transformDirty = true;
}

void SceneComponent::SetRelativeRotation(const FVector2D& rot)
{
	SetRelativeRotation(rot._x, rot._y, _relative._rotation._z);
}

void SceneComponent::SetRelativeRotation(float x, float y)
{
	SetRelativeRotation(x, y, _relative._rotation._z);
}

const FRotator& SceneComponent::AddRelativeRotation(const FRotator& rot)
{
	SetRelativeRotation(rot._x + _relative._rotation._x, rot._y + _relative._rotation._y, rot._z + _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotation(const FVector2D& rot)
{
	SetRelativeRotation(rot._x + _relative._rotation._x, rot._y + _relative._rotation._y, _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotation(const float x, float y, float z)
{
	SetRelativeRotation(x + _relative._rotation._x, y + _relative._rotation._y, z + _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotation(const float x, float y)
{
	SetRelativeRotation(x + _relative._rotation._x, y + _relative._rotation._y, _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotation(const float value)
{
	SetRelativeRotation(value + _relative._rotation._x, value + _relative._rotation._y, _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotationX(const float x)
{
	SetRelativeRotation(x + _relative._rotation._x, _relative._rotation._y, _relative._rotation._z);
	return _relative._rotation;
}

const FRotator& SceneComponent::AddRelativeRotationY(const float y)
{
	SetRelativeRotation(_relative._rotation._x, y + _relative._rotation._y, _relative._rotation._z);
	return _relative._rotation;
}

bool SceneComponent::ConsumeRenderBoundDirty()
{
	bool dirty = _renderBoundDirty;
	_renderBoundDirty = false;
	return dirty;
}

bool SceneComponent::BuildRenderBound2D(OUT FLocalBound2D& out) const
{
	return false;
}

bool SceneComponent::BuildBoundFromLocal(const FLocalBound2D& local, OUT FLocalBound2D& out) const
{
	out = FLocalBound2D{};

	FVector3D corners[4] = {
		{local._Min._x, local._Min._y, 0.f},
		{local._Min._x, local._Max._y, 0.f},
		{local._Max._x, local._Min._y, 0.f},
		{local._Max._x, local._Max._y, 0.f}
	};

	out.Reset();
	FMatrix world = GetWorldMatrix();
	for (const FVector3D& corner : corners)
	{
		FVector3D result = corner.TransformCoord(world);
		out.Expand(result._x, result._y);
	}
	return out.IsValid();
}

void SceneComponent::MarkRenderBoundDirty()
{
	_renderBoundDirty = true;
	if (!_isRender)
	{
		return;
	}

	Ptr<IComponentOwner> owner = GetOwner();
	if (!owner)
	{
		return;
	}

	RenderManager::Instance().MarkRenderBoundDirty(owner->GetOwnerID(), GetComponentID());
}

void SceneComponent::SetRenderLayerName(const std::string& name)
{
	
	if (_renderLayerName == name)
	{
		return;
	}

	_renderLayerName = name;
	_renderBoundDirty = true;
	MarkRenderBoundDirty();
	RenderManager::Instance().RefreshLayer();
}

void SceneComponent::UpdateTransform()
{
	_matrix._scale.Scaling(_relative._scale);
	_matrix._rotation.Rotation(_relative._rotation);
	_matrix._translate.Translation(_relative._position);

	// 크기 * 회전 * 이동
	FMatrix matLocal = _matrix._scale * _matrix._rotation * _matrix._translate;
	if (Ptr<SceneComponent> parentComp = Lock<SceneComponent>(_parent))
	{
		_matrix._world = matLocal * parentComp->_matrix._world;
	}
	else
	{
		_matrix._world = matLocal;
	}
	_matrix._world.ExtractPosition(_world._position);
	_matrix._world.ExtractEuler(_world._rotation);
	_matrix._world.ExtractScale(_world._scale);

	for (auto& it : _childs)
	{
		if (it.second)
		{
			it.second->_transformDirty = true;
		}
	}

	FVector3D axis[AXIS_TYPE::END] = {
		FVector3D(FVector3D::Axis_X),
		FVector3D(FVector3D::Axis_Y),
		FVector3D(FVector3D::Axis_Z)
	};

	_axis[AXIS_TYPE::X] = axis[AXIS_TYPE::X].TransformNormal(_matrix._rotation);
	_axis[AXIS_TYPE::Y] = axis[AXIS_TYPE::Y].TransformNormal(_matrix._rotation);
	_axis[AXIS_TYPE::Z] = axis[AXIS_TYPE::Z].TransformNormal(_matrix._rotation);

	MarkRenderBoundDirty();
}

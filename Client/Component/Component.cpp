#include "pch.h"
#include "Component.h"
#include "Actor.h"
#include "../Editor/EditEngine.h"


Component::Component()
{
}

Component::~Component()
{
}

const Ptr<IComponentOwner> Component::GetOwner()
{
	return Lock<IComponentOwner>(_owner);
}

void Component::SetOwner(Ptr<IComponentOwner> owner)
{
	_owner = owner;
	_level = owner->GetLevel();
}

eComponentType Component::GetComponentType() const
{
	return _type;
}

bool Component::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	_id = id;
	_name = name;
	_owner = owner;
	_level = owner->GetLevel();

	SetActive(true);
	SetEnable(true);
	return true;
}

void Component::Tick(float deltaTime)
{
}

void Component::Collision(float deltaTime)
{
}

void Component::Render(float deltaTime)
{
}

void Component::DrawInspector()
{
	ImGui::SeparatorText("Component");

	char name[128] = {};
	strcpy_s(name, 128, _name.c_str());
	ImGui::InputText("ComponentName", name, 128);
	SetName(name);

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
}

void Component::Destroy()
{
	SetEnable(false);
	SetActive(false);
}

bool Component::Save(std::ofstream& file)
{
	return true;
}

bool Component::Load(std::ifstream& file)
{
	return true;
}

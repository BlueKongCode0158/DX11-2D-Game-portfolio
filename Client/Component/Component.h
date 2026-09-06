#pragma once
#include "Object/Object.h"
#include "../Editor/Interface/Interface.h"
#include <string>

// 컴포넌트 패턴
// 부품이라고 볼 수 있다.
// 게임 오브젝트의 기능들을 각각의 컴포넌트 라는 단위로 나눠서 관리하고 개발하는 것을 말한다.
// ex) 외형 컴포넌트 (mesh), 사운드 컴포넌트 (sound), 입력 컴포넌트 (input)

// 컴포넌트 패턴 장점
// 1. 기능별로 컴포넌트를 나ㅝ서 관리하기 때문에 유지보수가 편하다.
// 2. 코드 재사용성이 높아진다.

// 몬스터, 플레이어
// mesh, animation, attack
// 몬스터 - 외형, 움직임, 공격 구현
// 플레이어 - 외형, 움직임, 공격 구현

// unreal 에서도 컴포넌트 패턴을 사용한다. - 대표적인 2가지. 
// 1. 씬 컴포넌트
// ---> 트랜스폼(위치, 회전, 크기) 정보를 가지고 있는 컴포넌트 
// MeshComponent --> 위치, 크기, 회전 등 트랜스폼 정보 필요
// 2. 액터 컴포넌트
// ---> 씬 컴포넌트를 제외한 나머지 기능들을 제공하는 컴포넌트
// AIComponent --> 기능만 있어.

// 컴포넌트도 자기가 누구에게 속해있는지 알아야 편하다.
class Component : public Object, public IArchive
{
	friend class Actor;
public:
	Component();
	virtual ~Component();

protected:
	// 순환참조를 막기 위해서 컴포넌트는 weak_ptr로 들고 있는다.
	Weak<IComponentOwner> _owner;
	Weak<class Level> _level;
	std::string _name;
	int32 _id = -1;
	eComponentType _type = eComponentType::END;
public:
	const std::string& GetName() { return _name; }
	void SetName(const std::string& name) { _name = name; }
	const int32 GetComponentID() const { return _id; }
	void SetComponentID(int32 id) { _id = id; }
	const Ptr<IComponentOwner> GetOwner();
	void SetOwner(Ptr<IComponentOwner> owner);
	eComponentType GetComponentType() const;
public:
	virtual bool Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner);
	virtual void Tick(float deltaTime);
	virtual void Collision(float deltaTime);
	virtual void Render(float deltaTime);
	virtual void DrawInspector() override;
	virtual void Destroy() override;
public:
	virtual bool Save(std::ofstream& file);
	virtual bool Load(std::ifstream& file);
}; 


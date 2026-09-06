#include "pch.h"
#include "../Collision/CollisionSystem.h"
#include "EdgeCollisionComponent.h"
#include "AABBCollisionComponent.h"
#include "../Object/Asset/AssetManager.h"
#include "../Object/Asset/Mesh/MeshManager.h"

EdgeCollisionComponent::EdgeCollisionComponent()
{
	// 엣지 콜라이더는 환경 정적 충돌선 전용이다. base Init 의 등록 시점에 이미
	// static 임을 알 수 있도록 생성자에서 켜 둔다(리버킷 불필요).
	_isStatic = true;
}

EdgeCollisionComponent::~EdgeCollisionComponent()
{
}

bool EdgeCollisionComponent::Init(int32 id, const std::string& name, Ptr<IComponentOwner> owner)
{
	CollisionComponent::Init(id, name, owner);

	_shape	= eCollisonShape::EDGE;
	_type	= eComponentType::EDGE;
#ifdef _DEBUG
	_mesh	= MESH_MANAGER->FindMesh("Line");
#endif // _DEBUG

	return true;
}

void EdgeCollisionComponent::Tick(float deltaTime)
{
	CollisionComponent::Tick(deltaTime);
}

void EdgeCollisionComponent::Collision(float deltaTime)
{
	CollisionComponent::Collision(deltaTime);
}

void EdgeCollisionComponent::Render(float deltaTime)
{
	CollisionComponent::Render(deltaTime);
}

void EdgeCollisionComponent::DrawInspector()
{
	CollisionComponent::DrawInspector();
}

void EdgeCollisionComponent::Destroy()
{
	CollisionComponent::Destroy();
}

bool EdgeCollisionComponent::Save(std::ofstream& file)
{
	return false;
}

bool EdgeCollisionComponent::Load(std::ifstream& file)
{
	return false;
}

bool EdgeCollisionComponent::Collision(Weak<CollisionComponent> dest)
{
	Ptr<CollisionComponent> destCol = Lock<CollisionComponent>(dest);
	if (!destCol)
	{
		return false;
	}

	switch (destCol->GetShape())
	{
	case eCollisonShape::AABB:
		return CollisionSystem::EdgeToAABB( This<EdgeCollisionComponent>(),
											Cast<CollisionComponent, AABBCollisionComponent>(destCol));
		break;
	case eCollisonShape::OBB:
		break;
	case eCollisonShape::SPHERE:
		break;
	case eCollisonShape::EDGE:
		return CollisionSystem::EdgeToEdge( This<EdgeCollisionComponent>(),
											Cast<CollisionComponent, EdgeCollisionComponent>(destCol));
		break;
	case eCollisonShape::END:
		break;
	default:
		break;
	}
	return false;
}

void EdgeCollisionComponent::SetLine(const FVector2D& startPoint, const FVector2D& endPoint)
{
	_Line._StartPoint = startPoint;
	_Line._EndPoint = endPoint;
	ReflashLinePoint();
}

void EdgeCollisionComponent::SetLine(FEDGE2D& line)
{
	_Line = line;
	ReflashLinePoint();
}

void EdgeCollisionComponent::ReflashLinePoint()
{
	const FVector2D& startPoint = _Line._StartPoint;
	const FVector2D& endPoint = _Line._EndPoint;

	FVector2D dir	= endPoint - startPoint;
	float length	= dir.Length();

	// 길이가 0.0001f 이하면 두 점이 같다고 판단하고
	// 리턴
	if (length < 1e-4f)
	{
		return;
	}

	FVector2D mindPoint = (startPoint + endPoint) * 0.5f;
	float angle = DirectX::XMConvertToDegrees(atan2f(dir._y, dir._x));
	
	SetWorldScale(length, 1.f);
	SetWorldRotation(0.f, 0.f, angle);
	SetWorldPosition(mindPoint);
	UpdateTransform();
}

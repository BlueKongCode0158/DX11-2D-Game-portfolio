#include "pch.h"
#include "Common/Info.h"
#include "CollisionSystem.h"
#include "Component/AABBCollisionComponent.h"
#include "Component/SphereCollisionComponent.h"
#include "Component/OBBCollisionComponent.h"
#include "Component/EdgeCollisionComponent.h"

bool CollisionSystem::AABBToAABB(Weak<class AABBCollisionComponent> src, Weak<class AABBCollisionComponent> dest)
{
	Ptr<AABBCollisionComponent> srcCol = Lock<AABBCollisionComponent>(src);
	Ptr<AABBCollisionComponent> destCol = Lock<AABBCollisionComponent>(dest);

	if (!srcCol || !destCol)
	{
		return false;
	}
	return AABBToAABB(srcCol->GetBox(), destCol->GetBox());
}

bool CollisionSystem::AABBToAABB(const FAABB2D& src, const FAABB2D& dest)
{
	if (src._min._x > dest._max._x)
	{
		return false;
	}
	else if (dest._min._x > src._max._x)
	{
		return false;
	}
	else if (src._min._y > dest._max._y)
	{
		return false;
	}
	else if (dest._min._y > src._max._y)
	{
		return false;
	}
	return true;
}

bool CollisionSystem::AABBToSphere(Weak<class AABBCollisionComponent> src, Weak<class SphereCollisionComponent> dest)
{
	Ptr<AABBCollisionComponent> srcCol = Lock<AABBCollisionComponent>(src);
	Ptr<SphereCollisionComponent> destCol = Lock <SphereCollisionComponent>(dest);

	if (!srcCol || !destCol)
	{
		return false;
	}

	return AABBToSphere(srcCol->GetBox(), destCol->GetWorldPosition(), destCol->GetRadius());
}

bool CollisionSystem::AABBToSphere(const FAABB2D& src, const FVector3D& destCenter, float destRadiuse)
{
	// 중점과 가까운 점을 찾는다.
	float foundX = Utility::Clamp(destCenter._x, src._min._x, src._max._x);
	float foundY = Utility::Clamp(destCenter._y, src._min._y, src._max._y);

	float distX = destCenter._x - foundX;
	float distY = destCenter._y - foundY;

	float foundDistSquare = (distX * distX) + (distY * distY);
	float radiuseSquare = destRadiuse * destRadiuse;
	
	return foundDistSquare <= radiuseSquare;
}

bool CollisionSystem::AABBToOBB(Weak<class AABBCollisionComponent> src, Weak<class OBBCollisionComponent> dest)
{
	Ptr<AABBCollisionComponent> srcCol= Lock<AABBCollisionComponent>(src);
	Ptr<OBBCollisionComponent> destCol = Lock<OBBCollisionComponent>(dest);
	if (!srcCol || !destCol)
	{
		return false;
	}

	return AABBToOBB(srcCol->GetBox(), destCol->GetBox());
}

bool CollisionSystem::AABBToOBB(const FAABB2D& srcAABB, const FOBB2D& destOBB)
{
	FOBB2D obb = CreateOBB(srcAABB);

	return OBBToOBB(obb, destOBB);
}

bool CollisionSystem::OBBToOBB(Weak<class OBBCollisionComponent> src, Weak<class OBBCollisionComponent> dest)
{
	Ptr<OBBCollisionComponent> srcCol = Lock<OBBCollisionComponent>(src);
	Ptr<OBBCollisionComponent> destCol = Lock<OBBCollisionComponent>(dest);
	if (!srcCol || !destCol)
	{
		return false;
	}

	return OBBToOBB(srcCol->GetBox(), destCol->GetBox());
}

bool CollisionSystem::OBBToOBB(const FOBB2D& srcOBB, const FOBB2D& destOBB)
{
	FVector2D centerLine = srcOBB._center - destOBB._center;
	if (!ComputeAxisProjection(centerLine, srcOBB._axis[AXIS_TYPE::X], srcOBB._halfSize._x, destOBB._axis, destOBB._halfSize))
	{
		return false;
	}
	if (!ComputeAxisProjection(centerLine, srcOBB._axis[AXIS_TYPE::Y], srcOBB._halfSize._y, destOBB._axis, destOBB._halfSize))
	{
		return false;
	}
	if (!ComputeAxisProjection(centerLine, destOBB._axis[AXIS_TYPE::X], destOBB._halfSize._x, srcOBB._axis, srcOBB._halfSize))
	{
		return false;
	}
	if (!ComputeAxisProjection(centerLine, destOBB._axis[AXIS_TYPE::Y], destOBB._halfSize._y, srcOBB._axis, srcOBB._halfSize))
	{
		return false;
	}

	return true;
}

bool CollisionSystem::OBBToSphere(Weak<class OBBCollisionComponent> src, Weak<class SphereCollisionComponent> dest)
{
	Ptr<OBBCollisionComponent> srcCol = Lock<OBBCollisionComponent>(src);
	Ptr<SphereCollisionComponent> destCol = Lock<SphereCollisionComponent>(dest);

	if (!srcCol || !destCol)
	{
		return false;
	}

	return OBBToSphere(srcCol->GetBox(), destCol->GetWorldPosition(), destCol->GetRadius());
}

bool CollisionSystem::OBBToSphere(const FOBB2D& srcOBB, const FVector3D& destCenter, float destRadius)
{
	FVector2D convertCenter;
	convertCenter._x = destCenter._x;
	convertCenter._y = destCenter._y;

	FVector2D centerLine = convertCenter - srcOBB._center;
	FVector2D axis = centerLine;
	axis.Normalize();

	float centerProj = std::abs(centerLine.Dot(axis));
	float obbProjOnAxis = std::abs(axis.Dot(srcOBB._axis[AXIS_TYPE::X])) * srcOBB._halfSize._x + std::abs(axis.Dot(srcOBB._axis[AXIS_TYPE::Y])) * srcOBB._halfSize._y;

	if (centerProj > destRadius + obbProjOnAxis)
	{
		return false;
	}

	float projX = std::abs(centerLine.Dot(srcOBB._axis[AXIS_TYPE::X]));
	if (projX > destRadius + srcOBB._halfSize._x)
	{
		return false;
	}

	float projY = std::abs(centerLine.Dot(srcOBB._axis[AXIS_TYPE::Y]));
	if (projY > destRadius + srcOBB._halfSize._y)
	{
		return false;
	}

	return true;
}

bool CollisionSystem::SphereToSphere(Weak<class SphereCollisionComponent> src, Weak<class SphereCollisionComponent> dest)
{
	Ptr<SphereCollisionComponent> srcCol = Lock<SphereCollisionComponent>(src);
	Ptr<SphereCollisionComponent> destCol = Lock<SphereCollisionComponent>(dest);
	
	if (!srcCol || !destCol)
	{
		return false;
	}
	return SphereToSphere(srcCol->GetWorldPosition(), srcCol->GetRadius(), destCol->GetWorldPosition(), destCol->GetRadius());
}

bool CollisionSystem::SphereToSphere(const FVector3D& srcCenter, float srcRadiuse, const FVector3D& destCenter, float destRadiuse)
{
	float dist = srcCenter.Distance(destCenter);

	if (dist > srcRadiuse + destRadiuse)
	{
		return false;
	}
	return true;
}

bool CollisionSystem::EdgeToEdge(Weak<class EdgeCollisionComponent> src, Weak<class EdgeCollisionComponent> dest)
{
	Ptr<EdgeCollisionComponent> srcEdge = Lock<EdgeCollisionComponent>(src);
	Ptr<EdgeCollisionComponent> destEdge = Lock<EdgeCollisionComponent>(dest);

	if (!srcEdge || !destEdge)
	{
		return false;
	}

	return EdgeToEdge(srcEdge->GetLine(), destEdge->GetLine());
}

bool CollisionSystem::EdgeToEdge(const FEDGE2D& srcEdge, const FEDGE2D& destEdge)
{
	const FVector2D& srcStartPoint = srcEdge._StartPoint;
	const FVector2D& destStartPoint = destEdge._StartPoint;

	// const reference가 임시 객체의 생명주기를 연장 but 그냥 값으로 받는 편이 깔끔
	const FVector2D srcDir = srcEdge._EndPoint - srcEdge._StartPoint;
	const FVector2D destDir = destEdge._EndPoint - destEdge._StartPoint;
	const FVector2D lineDir = destStartPoint - srcStartPoint;

	// 두 방향 벡터가 평행한지에 대한 값.
	const float cross = srcDir.Cross(destDir);

	const float lineCross = lineDir.Cross(srcDir);
	
	// 두 선분이 평행한 경우
	if (IsZero(cross))
	{
		// 평행하지만 같은 직선 위에 있지 않음.
		if (!IsZero(lineCross))
		{
			return false;
		}

		// 같은 직선에 위에 있으므로 AABB 범위 겹침 확인.
		const float srcMinX = min(srcEdge._StartPoint._x, srcEdge._EndPoint._x);
		const float srcMaxX = max(srcEdge._StartPoint._x, srcEdge._EndPoint._x);
		const float srcMinY = min(srcEdge._StartPoint._y, srcEdge._EndPoint._y);
		const float srcMaxY = max(srcEdge._StartPoint._y, srcEdge._EndPoint._y);

		const float destMinX = min(destEdge._StartPoint._x, destEdge._EndPoint._x);
		const float destMaxX = max(destEdge._StartPoint._x, destEdge._EndPoint._x);
		const float destMinY = min(destEdge._StartPoint._y, destEdge._EndPoint._y);
		const float destMaxY = max(destEdge._StartPoint._y, destEdge._EndPoint._y);

		const bool overlapX = (srcMaxX + EPSILON >= destMinX) && (destMaxX + EPSILON >= srcMinX);
		const bool overlapY = (srcMaxY + EPSILON >= destMinY) && (destMaxY + EPSILON >= srcMinY);

		return overlapX && overlapY;
	}

	const float crossDestLine	= lineDir.Cross(destDir) / cross;
	const float crossSrcLine	= lineDir.Cross(srcDir)  / cross;

	return crossDestLine >= -EPSILON && crossDestLine <= 1.f + EPSILON && crossSrcLine >= -EPSILON && crossSrcLine <= 1.f + EPSILON;
}

bool CollisionSystem::EdgeToAABB(Weak<class EdgeCollisionComponent> src, Weak<class AABBCollisionComponent> dest)
{
	Ptr<EdgeCollisionComponent> srcCol = Lock<EdgeCollisionComponent>(src);
	Ptr<AABBCollisionComponent> destCol = Lock<AABBCollisionComponent>(dest);

	if (!srcCol || !destCol)
	{
		return false;
	}

	return EdgeToAABB(srcCol->GetLine(), destCol->GetBox());
}

bool CollisionSystem::EdgeToAABB(const FEDGE2D& srcEdge, const FAABB2D& destAABB)
{

	if (IsPointInsideAABB(srcEdge._StartPoint, destAABB))
	{
		return true;
	}

	if (IsPointInsideAABB(srcEdge._EndPoint, destAABB))
	{
		return true;
	}

	FEDGEBOX2D boxEdge = CreateEdge(destAABB);

	if (EdgeToEdge(srcEdge, boxEdge._bottomEdge))
	{
		return true;
	}

	if (EdgeToEdge(srcEdge, boxEdge._rightEdge))
	{
		return true;
	}

	if (EdgeToEdge(srcEdge, boxEdge._topEdge))
	{
		return true;
	}

	if (EdgeToEdge(srcEdge, boxEdge._leftEdge))
	{
		return true;
	}

	return false;
}

bool CollisionSystem::Intersects(const FLocalBound2D& src, const FLocalBound2D& dest)
{
	if (src._Max._x < dest._Min._x)
	{
		return false;
	}
	if (src._Min._x > dest._Max._x)
	{
		return false;
	}
	if (src._Max._y < dest._Min._y)
	{
		return false;
	}
	if (src._Min._y > dest._Max._y)
	{
		return false;
	}
	return true;
}

bool CollisionSystem::ComputeAxisProjection(const FVector2D& centerLine, const FVector2D& srcAxis, float axisHalfSize, const FVector2D* destAxis, const FVector2D& destHalfSize)
{
	float centerProjectionDist = std::abs(centerLine.Dot(srcAxis));
	float destProjectionDist = std::abs(srcAxis.Dot(destAxis[AXIS_TYPE::X])) * destHalfSize._x + std::abs(srcAxis.Dot(destAxis[AXIS_TYPE::Y])) * destHalfSize._y;
	float overlap = axisHalfSize + destProjectionDist - centerProjectionDist;

	if (overlap <= 0)
	{
		return false;
	}
	return true;
}

bool CollisionSystem::IsPointInsideAABB(const FVector2D& srcPoint, const FAABB2D& destBox)
{
	return srcPoint._x >= destBox._min._x && srcPoint._x <= destBox._max._x
		&& srcPoint._y >= destBox._min._y && srcPoint._y <= destBox._max._y;
}

FOBB2D CollisionSystem::CreateOBB(const FAABB2D& aabb)
{
	FOBB2D ret;
	ret._center = (aabb._min + aabb._max) * 0.5f;
	ret._axis[AXIS_TYPE::X]._x = 1.f;
	ret._axis[AXIS_TYPE::X]._y = 0.f;

	ret._axis[AXIS_TYPE::Y]._x = 0.f;
	ret._axis[AXIS_TYPE::Y]._y = 1.f;

	ret._halfSize = (aabb._max - aabb._min) * 0.5f;
	return ret;
}

FEDGEBOX2D CollisionSystem::CreateEdge(const FAABB2D& aabb)
{
	const FVector2D leftBottom = FVector2D(aabb._min._x, aabb._min._y);
	const FVector2D rightBottom = FVector2D(aabb._max._x, aabb._min._y);
	const FVector2D leftTop = FVector2D(aabb._min._x, aabb._max._y);
	const FVector2D rightTop = FVector2D(aabb._max._x, aabb._max._y);

	FEDGEBOX2D boxEdge;
	boxEdge._bottomEdge._StartPoint = leftBottom;
	boxEdge._bottomEdge._EndPoint = rightBottom;

	boxEdge._leftEdge._StartPoint = leftTop;
	boxEdge._leftEdge._EndPoint = leftBottom;

	boxEdge._rightEdge._StartPoint = rightBottom;
	boxEdge._rightEdge._EndPoint = rightTop;

	boxEdge._topEdge._StartPoint = rightTop;
	boxEdge._topEdge._EndPoint = leftTop;

	return boxEdge;
}

bool CollisionSystem::IsZero(float value)
{
	return fabsf(value) <= EPSILON;
}

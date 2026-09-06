#pragma once
#include "Core/Defines.h"

class CollisionSystem
{
public:
	static bool AABBToAABB(Weak<class AABBCollisionComponent> src, Weak<class AABBCollisionComponent> dest);
	static bool AABBToAABB(const FAABB2D& src, const FAABB2D& dest);
	
	static bool AABBToSphere(Weak<class AABBCollisionComponent> src, Weak<class SphereCollisionComponent> dest);
	static bool AABBToSphere(const FAABB2D& src, const FVector3D& destCenter, float destRadiuse);
	
	static bool AABBToOBB(Weak<class AABBCollisionComponent> src, Weak<class OBBCollisionComponent> dest);
	static bool AABBToOBB(const FAABB2D& srcAABB, const FOBB2D& destOBB);

	static bool OBBToOBB(Weak<class OBBCollisionComponent> src, Weak<class OBBCollisionComponent> dest);
	static bool OBBToOBB(const FOBB2D& srcOBB, const FOBB2D& destOBB);
	
	static bool OBBToSphere(Weak<class OBBCollisionComponent> src, Weak<class SphereCollisionComponent> dest);
	static bool OBBToSphere(const FOBB2D& srcOBB, const FVector3D& destCenter, float destRadius);

	static bool SphereToSphere(Weak<class SphereCollisionComponent> src, Weak<class SphereCollisionComponent> dest);
	static bool SphereToSphere(const FVector3D& srcCenter, float srcRadiuse, const FVector3D& destCenter, float destRadiuse);

	static bool EdgeToEdge(Weak<class EdgeCollisionComponent> src, Weak<class EdgeCollisionComponent> dest);
	static bool EdgeToEdge(const FEDGE2D& srcEdge, const FEDGE2D& destEdge);

	static bool EdgeToAABB(Weak<class EdgeCollisionComponent> src, Weak<class AABBCollisionComponent> dest);
	static bool EdgeToAABB(const FEDGE2D& srcEdge, const FAABB2D& destAABB);

	static bool Intersects(const FLocalBound2D& src, const FLocalBound2D& dest);
private:
	static bool		ComputeAxisProjection(const FVector2D& centerLine, const FVector2D& srcAxis, float axisHalfSize, const FVector2D* destAxis, const FVector2D& destHalfSize);
	static bool		IsPointInsideAABB(const FVector2D& srcPoint, const FAABB2D& destBox);
	static bool		IsZero(float value);
	static FOBB2D		CreateOBB(const FAABB2D& aabb);
	static FEDGEBOX2D	CreateEdge(const FAABB2D& aabb);
private:
	inline static constexpr float EPSILON = 0.0001f;
};
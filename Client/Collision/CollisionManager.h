#pragma once
#include "Object/Object.h"

class CollisionManager : public Object
{
public:
	CollisionManager();
	virtual ~CollisionManager();
public:
	void Init();
	void Collision(float deltaTime);
	void Insert(std::pair<int32, int32>& colID, Ptr<class CollisionComponent> comp);
	void Remove(std::pair<int32, int32>& colID);
	Ptr<class CollisionComponent> FindCollider(std::pair<int32, int32>& colID);
public:
	virtual void Destroy() override;
private:
	// src(=동적) 콜라이더가 dst 와 충돌하는지 검사하고 상태 전이/콜백을 발화한다.
	// 충돌 콜백은 src 쪽에서만 발화하므로 정적 콜라이더는 dst 로만 검사한다.
	void TestPair(const std::pair<int32, int32>& srcKey, Ptr<class CollisionComponent>& src,
		const std::pair<int32, int32>& dstKey, Ptr<class CollisionComponent>& dst);
private:
	// 매 프레임 움직이는 콜라이더(플레이어/몬스터/공격 등). 서로 양방향으로 검사한다.
	std::map<std::pair<int32, int32>, Ptr<class CollisionComponent>> _dynamicColliders;
	// 환경 엣지 등 정적 콜라이더. 동적 콜라이더의 dst 로만 검사하고 정적끼리는 검사하지 않는다.
	std::map<std::pair<int32, int32>, Ptr<class CollisionComponent>> _staticColliders;
	std::vector<std::pair<int32, int32>> _removeColliders;
};
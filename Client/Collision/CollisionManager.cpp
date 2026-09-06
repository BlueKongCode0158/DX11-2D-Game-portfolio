#include "pch.h"
#include "CollisionManager.h"
#include "CollisionProfileManager.h"
#include "Component/CollisionComponent.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Init()
{
}

void CollisionManager::Collision(float deltaTime)
{
	for (auto it : _removeColliders)
	{
		_dynamicColliders.erase(it);
		_staticColliders.erase(it);
	}
	_removeColliders.clear();

	for (auto& it : _dynamicColliders)
	{
		const std::pair<int32, int32>& srcKey = it.first;
		Ptr<CollisionComponent> src = it.second;

		if (!src->IsEnable() || !src->IsActive())
		{
			continue;
		}

		if (nullptr == src->GetProfile())
		{
			continue;
		}

		// 동적 ↔ 동적 : 서로가 src/dst 가 되도록 양방향으로 검사한다.
		for (auto& subIt : _dynamicColliders)
		{
			if (srcKey == subIt.first)
			{
				continue;
			}
			TestPair(srcKey, src, subIt.first, subIt.second);
		}

		// 동적 → 정적 : 콜백은 src(동적) 쪽에서만 발화하므로 한 방향만 검사한다.
		// (정적끼리는 검사하지 않아 O(N^2) 의 정적-정적 비용을 제거한다.)
		for (auto& subIt : _staticColliders)
		{
			TestPair(srcKey, src, subIt.first, subIt.second);
		}
	}
}

void CollisionManager::TestPair(const std::pair<int32, int32>& srcKey, Ptr<class CollisionComponent>& src,
	const std::pair<int32, int32>& dstKey, Ptr<class CollisionComponent>& dst)
{
	if (!dst->IsEnable() || !dst->IsActive())
	{
		return;
	}

	Ptr<CollisionProfile> srcProfile = src->GetProfile();
	Ptr<CollisionProfile> dstProfile = dst->GetProfile();
	if (nullptr == dstProfile)
	{
		return;
	}

	if (srcProfile->GetResponse(dstProfile->GetChannel()) == eCollisionResponse::COLLISION_RESPONSE_IGNORE ||
		srcProfile->GetResponse(dstProfile->GetChannel()) == eCollisionResponse::COLLISION_RESPONSE_END)
	{
		return;
	}

	if (dstProfile->GetResponse(srcProfile->GetChannel()) == eCollisionResponse::COLLISION_RESPONSE_IGNORE ||
		dstProfile->GetResponse(srcProfile->GetChannel()) == eCollisionResponse::COLLISION_RESPONSE_END)
	{
		return;
	}

	std::pair<int32, int32> dstKeyCopy = dstKey;
	if (src->Collision(dst))
	{
		switch (src->CheckState(dstKeyCopy))
		{
		case eCollisionState::COLLISION_STATE_BLOCK:
		case eCollisionState::COLLISION_STATE_OVERLAP:
			src->Invoke(eCollisionState::COLLISION_STATE_OVERLAP, dst, dstKey);
			break;
		case eCollisionState::COLLISION_STATE_RELEASE:
			src->Invoke(eCollisionState::COLLISION_STATE_BLOCK, dst, dstKey);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (src->CheckState(dstKeyCopy))
		{
		case eCollisionState::COLLISION_STATE_BLOCK:
		case eCollisionState::COLLISION_STATE_OVERLAP:
			src->Invoke(eCollisionState::COLLISION_STATE_RELEASE, dst, dstKey);
			break;
		default:
			break;
		}
	}
}

void CollisionManager::Insert(std::pair<int32, int32>& colID, Ptr<class CollisionComponent> comp)
{
	if (comp->IsStatic())
	{
		_staticColliders[colID] = comp;
	}
	else
	{
		_dynamicColliders[colID] = comp;
	}
}

void CollisionManager::Remove(std::pair<int32, int32>& colID)
{
	_removeColliders.push_back(colID);
}

Ptr<class CollisionComponent> CollisionManager::FindCollider(std::pair<int32, int32>& colID)
{
	auto it = _dynamicColliders.find(colID);
	if (_dynamicColliders.end() != it)
	{
		return it->second;
	}

	auto staticIt = _staticColliders.find(colID);
	if (_staticColliders.end() != staticIt)
	{
		return staticIt->second;
	}

	return nullptr;
}

void CollisionManager::Destroy()
{
	_removeColliders.clear();
	_dynamicColliders.clear();
	_staticColliders.clear();
}

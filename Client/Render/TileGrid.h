#pragma once
#include "../Object/Object.h"
#include <unordered_set>

struct FRenderObjectKey
{
	int32 _actorID = -1;
	int32 _compID = -1;

	bool operator==(const FRenderObjectKey& other) const
	{
		return _actorID == other._actorID && _compID == other._compID;
	}
};

struct FRenderObjectKeyHash
{
	size_t operator()(const FRenderObjectKey& key) const
	{
		size_t h = static_cast<size_t>(static_cast<uint32>(key._actorID));
		h = h * 2654435761u ^ static_cast<uint32>(key._compID);
		return h;
	}
};

struct FGridCell
{
	int32 _x = 0;
	int32 _y = 0;

	bool operator == (const FGridCell& other) const
	{
		return _x == other._x && _y == other._y;
	}
};

struct FGridCellHash
{
	size_t operator()(const FGridCell& cell) const
	{
		size_t h = static_cast<size_t>(static_cast<uint32>(cell._x));
		h = h * 2654435761u ^ static_cast<uint32>(cell._y);
		return h;
	}
};

class TileGrid : public Object
{
public:
	TileGrid();
	virtual ~TileGrid();
public:
	bool Init(float cellSize = 1024.f);
	void Register(const FRenderObjectKey& key, const FLocalBound2D& bound);
	void Unregister(const FRenderObjectKey& key);
	void Query(const FLocalBound2D& viewBound, OUT std::vector< FRenderObjectKey>& out) const;
	void Clear();
public:
	virtual void Destroy();
private:
	FGridCell ToCell(float x, float y) const;
private:
	float _cellSize = 1024.f;
	std::unordered_map<FGridCell, std::unordered_set<FRenderObjectKey, FRenderObjectKeyHash>, FGridCellHash>  _cells;
	std::unordered_map<FRenderObjectKey, std::vector<FGridCell>, FRenderObjectKeyHash> _objectCells;
};
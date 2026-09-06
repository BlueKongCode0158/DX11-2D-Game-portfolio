#include "pch.h"
#include "TileGrid.h"

#include <unordered_set>

TileGrid::TileGrid()
{
}

TileGrid::~TileGrid()
{
}

bool TileGrid::Init(float cellSize)
{
	_cellSize = cellSize;
	return true;
}

void TileGrid::Register(const FRenderObjectKey& key, const FLocalBound2D& bound)
{
	Unregister(key);

	if (!bound.IsValid())
	{
		return;
	}

	FGridCell minCell = ToCell(bound._Min._x, bound._Min._y);
	FGridCell maxCell = ToCell(bound._Max._x, bound._Max._y);

	const int32 cellCountX = maxCell._x - minCell._x + 1;
	const int32 cellCountY = maxCell._y - minCell._y + 1;

	if (cellCountX <= 0 || cellCountY <= 0 /*|| cellCountX * cellCountY > 256*/)
	{
		return;
	}

	auto& ownedCells = _objectCells[key];

	for (int32 y = minCell._y; y <= maxCell._y; ++y)
	{
		for (int32 x = minCell._x; x <= maxCell._x; ++x)
		{
			FGridCell cell{ x,y };
			_cells[cell].insert(key);
			ownedCells.push_back(cell);
		}
	}
}

void TileGrid::Unregister(const FRenderObjectKey& key)
{
	auto it = _objectCells.find(key);
	if (_objectCells.end() == it)
	{
		return;
	}

	for (const FGridCell& cell : it->second)
	{
		auto cellIt = _cells.find(cell);
		if (cellIt == _cells.end())
		{
			continue;
		}

		auto& objectKeys = cellIt->second;
		objectKeys.erase(key);

		if (objectKeys.empty())
		{
			_cells.erase(cellIt);
		}
	}
	_objectCells.erase(it);
}

void TileGrid::Query(const FLocalBound2D& viewBound, OUT std::vector<FRenderObjectKey>& out) const
{
	std::unordered_set<FRenderObjectKey, FRenderObjectKeyHash> visited;

	FGridCell minCell = ToCell(viewBound._Min._x, viewBound._Min._y);
	FGridCell maxCell = ToCell(viewBound._Max._x, viewBound._Max._y);

	for (int32 y = minCell._y; y <= maxCell._y ; ++y)
	{
		for (int32 x = minCell._x; x <= maxCell._x; ++x)
		{
			auto it = _cells.find({ x,y });
			if (_cells.end() == it)
			{
				continue;
			}

			for (const FRenderObjectKey& key : it->second)
			{
				if (visited.insert(key).second)
				{
					out.push_back(key);
				}
			}
		}
	}
}

void TileGrid::Clear()
{
	_cells.clear();
	_objectCells.clear();
}

void TileGrid::Destroy()
{
	Clear();
}

FGridCell TileGrid::ToCell(float x, float y) const
{
	FGridCell  cell;
	cell._x = static_cast<int32>(floorf(x / _cellSize));
	cell._y = static_cast<int32>(floorf(y / _cellSize));
	return cell;
}

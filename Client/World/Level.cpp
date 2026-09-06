#include "pch.h"
#include "Level.h"
#include "World.h"
#include "TagManager.h"
#include "Object/Player.h"
#include "Object/Monster.h"
#include "../Object/Prob.h"
#include "../Object/LightActor.h"
#include "../Object/AnimActor.h"
#include "../Object/CameraZone.h"
#include "LogManager.h"
#include "CameraManager.h"
#include "Component/CameraComponent.h"
#include "Collision/CollisionManager.h"
#include "Component/CollisionComponent.h"
#include "../Component/AABBCollisionComponent.h"
#include "../Component/EdgeCollisionComponent.h"
#include "../Component/StaticMeshComponent.h"
#include "../Object/Asset/AssetManager.h"
#include "../UI/UIManager.h"

#include "../ThirdParty/nlohmann/json.hpp"
#include "../Core/DirectoryManager.h"

#include "../Core/GameEngine.h"


#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_set>

// LoadLineColliders 전용 헬퍼. 청크 단위 edge collider JSON을 읽어
// 엣지 콜라이더와 내부 채움(fill) 메시 데이터를 빌드한다.
struct FChunkCollisionBuildContext
{
public:
	float _fillZ = 100.f;
	float _pointCompareEpsilon = 0.0001f;
	float _fillUnitScale = static_cast<float>(UNIT);
	std::set<std::pair<int32, int32>> _filledTiles;
public:
	bool ReadEdgePoints(const nlohmann::json& edge, OUT std::vector<FVector2D>& outPoints) const
	{
		outPoints.clear();

		if (edge.contains("collider") &&
			edge["collider"].contains("shape") &&
			edge["collider"]["shape"].contains("points") &&
			edge["collider"]["shape"]["points"].is_array())
		{
			return ReadPointArray(edge["collider"]["shape"]["points"], ReadEdgeOffset(edge), OUT outPoints);
		}

		if (edge.contains("geometry") &&
			edge["geometry"].contains("points") &&
			edge["geometry"]["points"].is_array())
		{
			return ReadPointArray(edge["geometry"]["points"], ReadEdgeOffset(edge), OUT outPoints);
		}

		return false;
	}

	FVector2D ReadEdgeOffset(const nlohmann::json& edge) const
	{
		FVector2D offset;
		if (!edge.contains("collider") ||
			!edge["collider"].contains("offset") ||
			!edge["collider"]["offset"].is_object())
		{
			return offset;
		}

		const nlohmann::json& offsetJson = edge["collider"]["offset"];
		offset._x = offsetJson.value("x", 0.f);
		offset._y = offsetJson.value("y", 0.f);
		return offset;
	}

	bool ReadPointArray(const nlohmann::json& pointArray, const FVector2D& offset, OUT std::vector<FVector2D>& outPoints) const
	{
		outPoints.reserve(pointArray.size());

		for (const nlohmann::json& pointJson : pointArray)
		{
			if (!pointJson.is_object())
			{
				continue;
			}

			FVector2D localPoint;
			localPoint._x = pointJson.value("x", 0.f) + offset._x;
			localPoint._y = pointJson.value("y", 0.f) + offset._y;
			outPoints.push_back(localPoint);
		}

		return outPoints.size() >= 2;
	}

	bool IsSamePoint(const FVector2D& a, const FVector2D& b) const
	{
		return std::fabs(a._x - b._x) <= _pointCompareEpsilon &&
			std::fabs(a._y - b._y) <= _pointCompareEpsilon;
	}

	bool IsPointInsidePolygon(float x, float y, const std::vector<FVector2D>& points) const
	{
		bool inside = false;
		size_t prev = points.size() - 1;

		for (size_t cur = 0; cur < points.size(); ++cur)
		{
			const FVector2D& a = points[cur];
			const FVector2D& b = points[prev];

			if ((a._y > y) != (b._y > y))
			{
				const float crossX = (b._x - a._x) * (y - a._y) / (b._y - a._y) + a._x;
				if (x < crossX)
				{
					inside = !inside;
				}
			}

			prev = cur;
		}

		return inside;
	}

	void AddFilledCollisionTiles(const std::vector<FVector2D>& localPoints, int32 chunkTileX, int32 chunkTileY,
		int32 partitionX, int32 partitionY)
	{
		if (localPoints.size() < 4 || !IsSamePoint(localPoints.front(), localPoints.back()))
		{
			return;
		}

		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;

		for (const FVector2D& point : localPoints)
		{
			minX = (std::min)(minX, point._x);
			minY = (std::min)(minY, point._y);
			maxX = (std::max)(maxX, point._x);
			maxY = (std::max)(maxY, point._y);
		}

		const int32 startX = (std::max)(0, static_cast<int32>(std::floor(minX)));
		const int32 startY = (std::max)(0, static_cast<int32>(std::floor(minY)));
		const int32 endX = (std::min)(partitionX, static_cast<int32>(std::ceil(maxX)));
		const int32 endY = (std::min)(partitionY, static_cast<int32>(std::ceil(maxY)));

		for (int32 y = startY; y < endY; ++y)
		{
			for (int32 x = startX; x < endX; ++x)
			{
				if (IsPointInsidePolygon(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, localPoints))
				{
					_filledTiles.emplace(chunkTileX + x, chunkTileY + y);
				}
			}
		}
	}

	void BuildFillMeshData(OUT std::vector<FVertexColor>& vertices, OUT std::vector<uint32>& indices) const
	{
		vertices.reserve(_filledTiles.size() * 4);
		indices.reserve(_filledTiles.size() * 6);

		for (const std::pair<int32, int32>& tile : _filledTiles)
		{
			AddFilledTileQuad(tile.first, tile.second, OUT vertices, OUT indices);
		}
	}

	void AddFilledTileQuad(int32 tileX, int32 tileY,
		OUT std::vector<FVertexColor>& vertices, OUT std::vector<uint32>& indices) const
	{
		const uint32 baseIndex = static_cast<uint32>(vertices.size());
		const float minX = static_cast<float>(tileX) * _fillUnitScale;
		const float minY = static_cast<float>(tileY) * _fillUnitScale;
		const float maxX = minX + _fillUnitScale;
		const float maxY = minY + _fillUnitScale;
		const FVector4D black(0.f, 0.f, 0.f, 1.f);

		vertices.emplace_back(FVector3D(minX, maxY, 0.f), black);
		vertices.emplace_back(FVector3D(maxX, maxY, 0.f), black);
		vertices.emplace_back(FVector3D(minX, minY, 0.f), black);
		vertices.emplace_back(FVector3D(maxX, minY, 0.f), black);

		indices.push_back(baseIndex + 0);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 3);
		indices.push_back(baseIndex + 0);
		indices.push_back(baseIndex + 3);
		indices.push_back(baseIndex + 2);
	}
};

// Resources/Json 아래의 레벨 JSON 파일을 열어 파싱한다. 4개 Load* 함수가 공유하던
// 디렉토리 확인/파일 열기/파싱/discarded 검사 보일러플레이트를 한곳으로 모은다.
bool OpenLevelJson(const std::wstring& fileName, OUT nlohmann::json& root,
	OUT std::filesystem::path* outFilePath = nullptr)
{
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");
	if (!resPath.has_value())
	{
		return false;
	}

	std::filesystem::path jsonDir;
	if (!DirectoryManager::Instance().GetDirectory(resPath.value(), "Json", jsonDir))
	{
		return false;
	}

	std::filesystem::path filePath;
	if (!DirectoryManager::Instance().GetFile(jsonDir, fileName, filePath))
	{
		return false;
	}

	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	root = nlohmann::json::parse(file, nullptr, false);
	if (root.is_discarded())
	{
		return false;
	}

	if (outFilePath)
	{
		*outFilePath = filePath;
	}
	return true;
}

Level::Level()
{
}

Level::~Level()
{
}

bool Level::Init(const std::string& path)
{
	_tagManger = New<TagManager>();
	_tagManger->Init();
	// TODO : level save & load 하면 채워줘야한다.

	_collisionManager = New<CollisionManager>();
	_collisionManager->Init();

	_cameraManager = GetCameraManager();

	_uiManager = New<UIManager>();
	_uiManager->Init(This<Level>());

	if (!path.empty())
	{
		auto cachePath = DirectoryManager::Instance().GetCachePath("Resources\\Level");
		if (!cachePath.has_value() || path.empty())
		{
			return false;
		}

		std::filesystem::path fullPath;
		if (!DirectoryManager::Instance().GetFile(cachePath.value(), path, OUT fullPath))
		{
			return false;
		}

		std::ifstream loadFile(fullPath, std::ios::binary);
		if (loadFile.fail())
		{
			return false;
		}
		Load(loadFile);
	}
	return true;
}

void Level::Tick(float deltaTime)
{
	for (auto it : _removeActors)
	{
		Ptr<Actor> actor = FindActor<Actor>(it);
		if (nullptr == actor)
		{
			continue;
		}
		actor->Destroy();
		Delete(actor);
		_actors.erase(it);
	}
	_removeActors.clear();

	for (auto& it : _actors)
	{
		if (false == it.second->IsActive())
		{
			RemoveActor(it.first);
		}
		if (false == it.second->IsEnable())
		{
			continue;
		}
		it.second->Tick(deltaTime);
	}

	// 만약 레벨에 컴포넌트 붙일 일 있으면 여기서 실행됨.
	for (auto& it : _levelComponents)
	{
		if (it.second && it.second->IsEnable() && it.second->IsActive())
		{
			it.second->Tick(deltaTime);
		}
	}

}

void Level::Collision(float deltaTime)
{
	_collisionManager->Collision(deltaTime);

	for (auto& it : _actors)
	{
		if (!it.second->IsActive())
		{
			RemoveActor(it.first);
			continue;
		}
		if (!it.second->IsEnable())
		{
			continue;
		}

		it.second->Collision(deltaTime);
	}
	Lock<CameraManager>(_cameraManager)->Tick(deltaTime);

}

void Level::Render(float deltaTime)
{
	for (auto& it : _actors)
	{
		if (false == it.second->IsActive())
		{
			RemoveActor(it.first);
			continue;
		}

		if (false == it.second->IsEnable())
		{
			continue;
		}

		it.second->Render(deltaTime);
	}
}

void Level::RenderUI(float deltaTime)
{
	_uiManager->Render();
}

void Level::Destroy()
{
	for (auto& it : _actors)
	{
		DESTROY(it.second);
	}
	_actors.clear();

	for (auto& it : _levelComponents)
	{
		DESTROY(it.second);
	}
	_levelComponents.clear();
	_levelComponentsFinders.clear();

	// 정적 엣지 콜라이더는 _collisionManager 파괴 전에 정리한다(각 Destroy 가
	// level->RemoveCollision 으로 매니저에서 자기 자신을 제거한다).
	for (auto& edge : _staticEdgeColliders)
	{
		DESTROY(edge);
	}
	_staticEdgeColliders.clear();

	DESTROY(_tagManger);
	DESTROY(_collisionManager);
	DESTROY(_uiManager);
}

bool Level::Save(std::ofstream& file)
{
	for (auto& it : _actors)
	{
		if (!it.second->IsActive())
		{
			continue;
		}

		it.second->Save(file);
	}
	return true;
}

bool Level::Load(std::ifstream& file)
{
	for (auto& it : _actors)
	{
		if (!it.second->IsActive())
		{
			continue;
		}
		it.second->Load(file);
	}
	return true;
}

Ptr<Level> Level::GetLevel()
{
	return This<Level>();
}

int32 Level::GetOwnerID()
{
	return -1;
}

Ptr<class Actor> Level::GetPlayer() const
{
	return Lock<Actor>(_Player);
}

void Level::SetPlayer(Ptr<class Actor> actor)
{
	if (!GetPlayer())
	{
		_Player = actor;
	}
}

Ptr<class CameraManager> Level::GetCameraManager()
{
	Ptr<class World> _curWorld = GameEngine::Instance().GetWorld();
	if (!_curWorld)
	{
		return nullptr;
	}
	return _curWorld->GetCamaeraManager();
}

void Level::SetLevelBound(FLocalBound2D localBound)
{
	_worldBound = localBound;
}

FLocalBound2D Level::GetLevelBound() const
{
	return _worldBound;
}

void Level::AddTag(const std::string& tag, int32 id)
{
	_tagManger->Add(tag, id);
}

void Level::DeleteTag(Ptr<class Actor> actor)
{
	if (actor->_tags.size() < 1)
	{
		return;
	}
	for (auto& it : actor->_tags)
	{
		_tagManger->Erase(it, actor->_id);
	}
}

Ptr<class Actor> Level::FindActor(int32 id)
{
	auto it = _actors.find(id);
	if (_actors.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

void Level::FindActors(const std::string& tag, OUT std::vector<Ptr<class Actor>>& outArr)
{
	std::vector<int32> actorIDs;
	_tagManger->GetActorIDs(tag, actorIDs);

	if (actorIDs.size() < 1)
	{
		return;
	}

	for (auto i : actorIDs)
	{
		Ptr<Actor> actor = FindActor(i);
		if (nullptr == actor)
		{
			continue;
		}
		outArr.push_back(actor);
	}
}

void Level::RemoveActor(int32 id)
{
	_removeActors.push_back(id);
}

void Level::SetMainCamera(Ptr<class CameraComponent> camera)
{
	Lock<CameraManager>(_cameraManager)->SetMainCamera(camera);
}

Ptr<class CameraComponent> Level::GetMainCamera() const
{
	return Lock<CameraManager>(_cameraManager)->GetMainCamera();
}

const FMatrix& Level::GetWorldMatrix()
{
	return Lock<CameraManager>(_cameraManager)->GetWorldMatrix();
}

const FMatrix& Level::GetViewMatrix()
{
	return Lock<CameraManager>(_cameraManager)->GetViewMatrix();
}

const FMatrix& Level::GetProjMatrix()
{
	return Lock<CameraManager>(_cameraManager)->GetProjMatrix();
}

const FMatrix& Level::GetUIProjMatrix()
{
	return Lock<CameraManager>(_cameraManager)->GetUIProjMatrix();
}

const FVector3D& Level::GetCameraWorldPos()
{
	return Lock<CameraManager>(_cameraManager)->GetCameraWorldPos();
}

void Level::AddCollision(std::pair<int32, int32>& colID, Ptr<class CollisionComponent> comp)
{
	_collisionManager->Insert(colID, comp);
}

Ptr<class CollisionComponent> Level::FindCollider(std::pair<int32, int32>& colID)
{
	return _collisionManager->FindCollider(colID);
}

void Level::RemoveCollision(std::pair<int32, int32>& colID)
{
	_collisionManager->Remove(colID);
}

const std::map<int32, Ptr<class Actor>>& Level::GetActors()
{
	return _actors;
}

bool Level::LoadProbInstance(const std::wstring& fileName)
{
	nlohmann::json root;
	if (!OpenLevelJson(fileName, OUT root))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}

	for (const nlohmann::json& item : root)
	{
		if (!item.contains("engine_transform"))
		{
			continue;
		}

		if (!item.contains("prob"))
		{
			continue;
		}

		if (!item.contains("renderer"))
		{
			continue;
		}

		const nlohmann::json& engineTransform = item["engine_transform"];
		const nlohmann::json& probData = item["prob"];
		const nlohmann::json& LocalData = item["unity_transform"];
		const nlohmann::json& colorData = item["renderer"];
		const nlohmann::json& meshData = item["mesh_match"];

		if (!engineTransform.contains("position") ||
			!engineTransform.contains("scale") ||
			!engineTransform.contains("rotation"))
		{
			continue;
		}

		const nlohmann::json& posJson = engineTransform["position"];
		const nlohmann::json& scaleJson = engineTransform["scale"];
		const nlohmann::json& rotJson = engineTransform["rotation"];
		const nlohmann::json& localScale = LocalData["local_scale"];

		FVector3D position, rotation, scale;
		position._x = posJson.value("x", 0.f);
		position._y = posJson.value("y", 0.f);
		position._z = posJson.value("z", 0.f);

		rotation._x = rotJson.value("pitch", 0.f);
		rotation._y = rotJson.value("yaw", 0.f);
		rotation._z = rotJson.value("roll", 0.f);

		scale._x = scaleJson.value("x", 1.f);
		scale._y = scaleJson.value("y", 1.f);
		scale._z = scaleJson.value("z", 1.f);

		const std::string objectName = item.value("object_name", "");

		const nlohmann::json& tintJson = colorData["color"];
		const nlohmann::json& meshJson = meshData["mesh"];

		int32 pathID = probData.value("id", 0);
		std::string spriteName = meshJson.value("texture_key", "");
		std::string meshKey = probData.value("mesh_key", "");
		std::string atlasName = probData.value("atlas_name", "") ;
		std::string layerName = probData.value("layer_name", "Default");
		int atlasIndex = probData.value("texture_index", 0);

		FVector4D Tint;
		Tint._x = tintJson.value("r", 1.f);
		Tint._y = tintJson.value("g", 1.f);
		Tint._z = tintJson.value("b", 1.f);
		Tint._w = tintJson.value("a", 1.f);

		if (meshKey.empty() || atlasName.empty() || pathID == 0)
		{
			continue;
		}

		Ptr<Prob> prob = SpawnActor<Prob>(objectName, position, scale, rotation);
		if (!prob)
		{
			continue;
		}

		prob->SetMesh(meshKey, pathID);
		prob->SetTexture(spriteName, atlasIndex);

		const std::string& name = item.value("object_name", "");

		if (name.find("fog") != std::string::npos || name.find("Fog") != std::string::npos)
		{
			prob->SetBlendMode(eBlendMode::LIGHT);
			prob->SetShader("FogShader");
		}
		prob->SetTint(Tint);

		// 부모(root) 월드 위치를 패럴렉스 스케일 계산용으로 전달한다.
		// group.mode == "parent" 인 경우 root_engine_transform.position 이 부모 위치.
		if (item.contains("group"))
		{
			const nlohmann::json& group = item["group"];
			if (group.contains("root_engine_transform") &&
				group["root_engine_transform"].contains("position"))
			{
				const nlohmann::json& parentPosJson = group["root_engine_transform"]["position"];
				FVector3D parentPos;
				parentPos._x = parentPosJson.value("x", 0.f);
				parentPos._y = parentPosJson.value("y", 0.f);
				parentPos._z = parentPosJson.value("z", 0.f);
				prob->SetParallaxParentPosition(parentPos);
			}
		}
	}

	return true;
}

bool Level::LoadLineColliders(const std::wstring& fileName)
{
	nlohmann::json root;
	std::filesystem::path filePath;
	if (!OpenLevelJson(fileName, OUT root, OUT &filePath))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}

	FChunkCollisionBuildContext buildContext;

	for (const nlohmann::json& entry : root)
	{
		if (!entry.is_object() || !entry.contains("layers") || !entry["layers"].is_array())
		{
			continue;
		}

		const float unitScale = entry.value("unit_scale", static_cast<float>(UNIT));
		buildContext._fillUnitScale = unitScale;

		const nlohmann::json emptyObject = nlohmann::json::object();
		const nlohmann::json& partitionSize =
			entry.contains("partition_size") && entry["partition_size"].is_object()
			? entry["partition_size"]
			: emptyObject;

		const nlohmann::json& tilemapSize =
			entry.contains("tilemap_size") && entry["tilemap_size"].is_object()
			? entry["tilemap_size"]
			: emptyObject;

		const int32 partitionX = (std::max)(1, partitionSize.value("x", 32));
		const int32 partitionY = (std::max)(1, partitionSize.value("y", 32));
		const float tilemapWidth = tilemapSize.value("width", static_cast<float>(partitionX));
		const int32 chunksX = (std::max)(1, static_cast<int32>(std::ceil(tilemapWidth / static_cast<float>(partitionX))));

		for (const nlohmann::json& layer : entry["layers"])
		{
			if (!layer.is_object() || !layer.contains("chunks") || !layer["chunks"].is_array())
			{
				continue;
			}

			for (const nlohmann::json& chunk : layer["chunks"])
			{
				if (!chunk.is_object() || !chunk.contains("edge_colliders") || !chunk["edge_colliders"].is_array())
				{
					continue;
				}

				const int32 chunkIndex = chunk.value("chunk_index", 0);
				const int32 chunkX = chunkIndex % chunksX;
				const int32 chunkY = chunkIndex / chunksX;
				const int32 chunkTileX = chunkX * partitionX;
				const int32 chunkTileY = chunkY * partitionY;

				for (const nlohmann::json& edgeData : chunk["edge_colliders"])
				{
					std::vector<FVector2D> localPoints;
					if (!buildContext.ReadEdgePoints(edgeData, OUT localPoints))
					{
						continue;
					}

					std::vector<FVector2D> worldPoints;
					worldPoints.reserve(localPoints.size());

					for (const FVector2D& localPoint : localPoints)
					{
						FVector2D worldPoint;
						worldPoint._x = (static_cast<float>(chunkTileX) + localPoint._x) * unitScale;
						worldPoint._y = (static_cast<float>(chunkTileY) + localPoint._y) * unitScale;
						worldPoints.push_back(worldPoint);
					}

					if (worldPoints.size() < 2)
					{
						continue;
					}

					const std::string edgeName = edgeData.value("name", "ChunkEdge");
					const int32 colliderID = edgeData.value("collider_id", 0);
					for (size_t i = 0; i + 1 < worldPoints.size(); ++i)
					{
						if (buildContext.IsSamePoint(worldPoints[i], worldPoints[i + 1]))
						{
							continue;
						}

						AddEdgeCollider(edgeName + "_" + std::to_string(colliderID) + "_" + std::to_string(i),
							worldPoints[i], worldPoints[i + 1]);
					}

					buildContext.AddFilledCollisionTiles(localPoints, chunkTileX, chunkTileY, partitionX, partitionY);
				}
			}
		}
	}

	if (buildContext._filledTiles.empty())
	{
		return true;
	}

	std::vector<FVertexColor> vertices;
	std::vector<uint32> indices;
	buildContext.BuildFillMeshData(OUT vertices, OUT indices);

	const std::string meshName = "ChunkCollisionFill_" + filePath.stem().string();
	Ptr<MeshManager> meshManager = MESH_MANAGER;
	if (!meshManager)
	{
		return false;
	}

	if (!meshManager->FindMesh(meshName))
	{
		if (!meshManager->CreateMesh(meshName,
			vertices.data(), sizeof(FVertexColor), static_cast<int32>(vertices.size()),
			D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			indices.data(), sizeof(uint32), static_cast<int32>(indices.size()), DXGI_FORMAT_R32_UINT))
		{
			return false;
		}
	}

	Ptr<Actor> fillActor = SpawnActor<Actor>(
		meshName,
		FVector3D(0.f, 0.f, buildContext._fillZ),
		FVector3D(1.f, 1.f, 1.f),
		FRotator(0.f, 0.f, 0.f)
	);

	if (!fillActor)
	{
		return false;
	}

	Ptr<StaticMeshComponent> fillMesh = fillActor->CreateSceneComponent<StaticMeshComponent>("ChunkCollisionFillMesh");
	fillMesh->SetMesh(meshName);
	fillMesh->SetShader("ColorMeshShader");
	// CreateMesh 가 슬롯에 자동 부여한 DefaultMaterial(TEXCOORD 요구 PS)을 색상 전용
	// 머티리얼로 교체한다. 안 그러면 SetMaterial 단계에서 PS 가 덮어써져 #342 링키지 에러로
	// fill 메시 DrawIndexed 가 통째로 드롭된다Chunck.
	fillMesh->SetMaterial(0, "ColorMeshMaterial");

	fillMesh->SetRenderLayerName("Chunck");
	fillActor->SetRootComponent(fillMesh);

	return true;
}

bool Level::LoadLightObject(const std::wstring& fileName)
{
	nlohmann::json root;
	if (!OpenLevelJson(fileName, OUT root))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}
	for (const nlohmann::json& item : root)
	{
		if (!item.is_object())
		{
			continue;
		}

		if (!item.contains("engine_transform") ||
			!item.contains("renderer") ||
			!item.contains("mesh_match"))
		{
			continue;
		}

		const nlohmann::json& engineTransform = item["engine_transform"];
		const nlohmann::json& rendererData = item["renderer"];
		const nlohmann::json& meshMatchData = item["mesh_match"];

		if (!engineTransform.contains("position") ||
			!engineTransform.contains("scale") ||
			!engineTransform.contains("rotation"))
		{
			continue;
		}

		if (!rendererData.contains("color"))
		{
			continue;
		}

		if (!meshMatchData.contains("mesh"))
		{
			continue;
		}

		const nlohmann::json& posJson = engineTransform["position"];
		const nlohmann::json& scaleJson = engineTransform["scale"];
		const nlohmann::json& rotJson = engineTransform["rotation"];
		const nlohmann::json& tintJson = rendererData["color"];
		const nlohmann::json& meshJson = meshMatchData["mesh"];

		FVector3D position;
		position._x = posJson.value("x", 0.f);
		position._y = posJson.value("y", 0.f);
		position._z = posJson.value("z", 0.f);

		FVector3D scale;
		scale._x = scaleJson.value("x", 1.f);
		scale._y = scaleJson.value("y", 1.f);
		scale._z = scaleJson.value("z", 1.f);

		FVector3D rotation;
		rotation._x = rotJson.value("pitch", 0.f);
		rotation._y = rotJson.value("yaw", 0.f);
		rotation._z = rotJson.value("roll", 0.f);

		FVector4D tint;
		tint._x = tintJson.value("r", 1.f);
		tint._y = tintJson.value("g", 1.f);
		tint._z = tintJson.value("b", 1.f);
		tint._w = tintJson.value("a", 1.f);

		const std::string objectName = item.value("object_name", "LightMask");
		nlohmann::json emptyObject = nlohmann::json::object();

		const nlohmann::json& runtimeLoad =
			item.contains("runtime_load") && item["runtime_load"].is_object()
			? item["runtime_load"]
			: emptyObject;

		const nlohmann::json& probData =
			item.contains("prob") && item["prob"].is_object()
			? item["prob"]
			: emptyObject;

		std::string meshKey = runtimeLoad.value("mesh_key", "");
		int32 meshPathID = runtimeLoad.value("mesh_path_id", 0);
		std::string textureKey = runtimeLoad.value("texture_key", "");


		Ptr<LightActor> lightMask = SpawnActor<LightActor>(
			objectName,
			position,
			scale,
			rotation
		);

		if (!lightMask)
		{
			continue;
		}

		lightMask->SetMesh(meshKey, meshPathID);
		lightMask->SetTexture(textureKey, 0);
		lightMask->SetTint(tint);
	}

	return true;
}

bool Level::LoadAnimationActor(const std::wstring& fileName)
{
	nlohmann::json root;
	if (!OpenLevelJson(fileName, OUT root))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}

	for (const nlohmann::json& item : root)
	{
		if (!item.is_object())
		{
			continue;
		}

		if (!item.contains("engine_transform") ||
			!item.contains("animation_type") ||
			!item.contains("runtime_animation") ||
			!item.contains("tk2d_sprite_components"))
		{
			continue;
		}

		const nlohmann::json& engineTransform = item["engine_transform"];
		const nlohmann::json& runtimeData = item["runtime_animation"];
		const nlohmann::json& animType = item["animation_type"];
		const nlohmann::json& spriteRender = item["tk2d_sprite_components"];

		if (!engineTransform.contains("position") ||
			!engineTransform.contains("scale") ||
			!engineTransform.contains("rotation"))
		{
			continue;
		}

		if (animType.empty())
		{
			continue;
		}

		const nlohmann::json& posJson = engineTransform["position"];
		const nlohmann::json& scaleJson = engineTransform["scale"];
		const nlohmann::json& rotJson = engineTransform["rotation"];

		FVector4D tint{ 1.f, 1.f, 1.f, 1.f };   // 기본값 흰색

		if (spriteRender.is_array() &&
			!spriteRender.empty() &&
			spriteRender[0].is_object())
		{
			const nlohmann::json& comp0 = spriteRender[0];

			if (comp0.contains("color") && comp0["color"].is_object())
			{
				const nlohmann::json& tintJson = comp0["color"];
				tint._x = tintJson.value("r", 1.f);
				tint._y = tintJson.value("g", 1.f);
				tint._z = tintJson.value("b", 1.f);
				tint._w = tintJson.value("a", 1.f);
			}
		}

		FVector3D position;
		position._x = posJson.value("x", 0.f);
		position._y = posJson.value("y", 0.f);
		position._z = posJson.value("z", 0.f);

		FVector3D scale;
		scale._x = scaleJson.value("x", 1.f);
		scale._y = scaleJson.value("y", 1.f);
		scale._z = scaleJson.value("z", 1.f);

		FVector3D rotation;
		rotation._x = rotJson.value("pitch", 0.f);
		rotation._y = rotJson.value("yaw", 0.f);
		rotation._z = rotJson.value("roll", 0.f);

		const std::string objectName = item.value("name", "LightMask");
		nlohmann::json emptyObject = nlohmann::json::object();

		const nlohmann::json& runtimeLoad =
			item.contains("runtime_animation") && item["runtime_animation"].is_object()
			? item["runtime_animation"]
			: emptyObject;

		const std::string animName = runtimeLoad.value("default_animation_name", "");
		if (animName.empty())
		{
			continue;
		}

		Ptr<AnimActor> animActor = SpawnActor<AnimActor>(
			objectName,
			position,
			scale,
			rotation
		);

		if (!animActor)
		{
			continue;
		}

		animActor->SetTint(tint);
		animActor->AddAnimation(animName);
	}

	return true;
}

bool Level::LoadColliderObject(const std::wstring& fileName)
{
	nlohmann::json root;
	if (!OpenLevelJson(fileName, OUT root))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}

	for (const nlohmann::json& item : root)
	{
		// visuals 는 배열이다. 객체가 아니다.
		if (!item.contains("visuals") || !item["visuals"].is_array())
		{
			continue;
		}

		if (!item.contains("colliders") || !item["colliders"].is_array())
		{
			continue;
		}

		// name 은 visual 원소가 아니라 item 의 속성이므로 루프 밖에서 읽는다.
		const std::string objectName = item.value("name", "");

		FVector2D minPoint;
		FVector2D maxPoint;
		std::string type;

		for (const nlohmann::json& colData : item["colliders"])
		{
			if (!colData.contains("level_geometry"))
			{
				continue;
			}
			const nlohmann::json& levelCollider = colData["level_geometry"];
			type = levelCollider.value("type", "");

			const nlohmann::json& boxData = levelCollider["aabb"];
			const nlohmann::json& minData = boxData["min"];
			minPoint._x = minData.value("x", 0.f);
			minPoint._y = minData.value("y", 0.f);

			const nlohmann::json& maxData = boxData["max"];
			maxPoint._x = maxData.value("x", 1.f);
			maxPoint._y = maxData.value("y", 1.f);
		}

		// visuals 배열의 각 원소를 순회한다. 이 원소가 기존의 probData 역할.
		for (const nlohmann::json& probData : item["visuals"])
		{
			if (!probData.contains("engine_transform") ||
				!probData.contains("renderer") ||
				!probData.contains("mesh"))
			{
				continue;
			}

			const nlohmann::json& engineTransform = probData["engine_transform"];
			const nlohmann::json& renderData = probData["renderer"];
			const nlohmann::json& meshData = probData["mesh"];

			if (!renderData.contains("color"))
			{
				continue;
			}

			const nlohmann::json& colorData = renderData["color"];

			if (!engineTransform.contains("position") ||
				!engineTransform.contains("scale") ||
				!engineTransform.contains("rotation"))
			{
				continue;
			}

			const nlohmann::json& posJson = engineTransform["position"];
			const nlohmann::json& scaleJson = engineTransform["scale"];
			const nlohmann::json& rotJson = engineTransform["rotation"];

			FVector3D position, rotation, scale;
			position._x = posJson.value("x", 0.f);
			position._y = posJson.value("y", 0.f);
			position._z = posJson.value("z", 0.f);

			rotation._x = rotJson.value("pitch", 0.f);
			rotation._y = rotJson.value("yaw", 0.f);
			rotation._z = rotJson.value("roll", 0.f);

			scale._x = scaleJson.value("x", 1.f);
			scale._y = scaleJson.value("y", 1.f);
			scale._z = scaleJson.value("z", 1.f);

			int32 pathID = meshData.value("sprite_path_id", 0);
			std::string spriteName = meshData.value("texture_key", "");
			std::string meshKey = meshData.value("mesh_key", "");
			std::string atlasName = meshData.value("atlas_name", "");
			int atlasIndex = meshData.value("texture_index", 0);

			FVector4D Tint;
			Tint._x = colorData.value("r", 1.f);
			Tint._y = colorData.value("g", 1.f);
			Tint._z = colorData.value("b", 1.f);
			Tint._w = colorData.value("a", 1.f);

			if (meshKey.empty() || atlasName.empty() || pathID == 0)
			{
				continue;
			}

			Ptr<Prob> prob = SpawnActor<Prob>(objectName, position, scale, rotation);
			if (!prob)
			{
				continue;
			}

			prob->SetMesh(meshKey, pathID);
			prob->SetTexture(spriteName, atlasIndex);
			prob->SetTint(Tint);


			if (type == "BoxCollider2D")
			{
				Ptr<AABBCollisionComponent> col = prob->CreateSceneComponent<AABBCollisionComponent>("Tile");
				col->SetCollisionProfile("Environment");

				col->SetBoxSize(maxPoint._x - minPoint._x, maxPoint._y - minPoint._y);
				col->AttachToComponent(prob->GetRoot());
			}


			// 부모(root) 월드 위치를 패럴렉스 스케일 계산용으로 전달한다.
			// group.mode == "parent" 인 경우 root_engine_transform.position 이 부모 위치.
			// (이 콜라이더 JSON 에는 group 이 없으므로 보통 실행되지 않지만, 안전하게 유지한다.)
			if (item.contains("group"))
			{
				const nlohmann::json& group = item["group"];
				if (group.contains("root_engine_transform") &&
					group["root_engine_transform"].contains("position"))
				{
					const nlohmann::json& parentPosJson = group["root_engine_transform"]["position"];
					FVector3D parentPos;
					parentPos._x = parentPosJson.value("x", 0.f);
					parentPos._y = parentPosJson.value("y", 0.f);
					parentPos._z = parentPosJson.value("z", 0.f);
					prob->SetParallaxParentPosition(parentPos);
				}
			}
		}
	}

	return true;
}

bool Level::LoadCameraLock(const std::wstring& fileName)
{
	nlohmann::json root;
	if (!OpenLevelJson(fileName, OUT root))
	{
		return false;
	}

	if (!root.is_array())
	{
		return false;
	}

	for (const nlohmann::json& item : root)
	{
		// visuals 는 배열이다. 객체가 아니다.
		if (!item.contains("camera_lock_zones") || !item["camera_lock_zones"].is_array())
		{
			continue;
		}


		// name 은 visual 원소가 아니라 item 의 속성이므로 루프 밖에서 읽는다.
		const std::string objectName = item.value("name", "");

		FVector2D minPoint;
		FVector2D maxPoint;
		std::string type;

		for (const nlohmann::json& camLockData : item["camera_lock_zones"])
		{
			if (!camLockData.contains("trigger_geometry"))
			{
				continue;
			}

			type = camLockData.value("type", "");
			const nlohmann::json& levelCollider = camLockData["trigger_geometry"];

			type = levelCollider.value("type", "");

			const nlohmann::json& boxData = levelCollider["aabb"];
			const nlohmann::json& minData = boxData["min"];
			minPoint._x = minData.value("x", 0.f);
			minPoint._y = minData.value("y", 0.f);

			const nlohmann::json& maxData = boxData["max"];
			maxPoint._x = maxData.value("x", 1.f);
			maxPoint._y = maxData.value("y", 1.f);

			if (!camLockData.contains("engine_transform"))
			{
				continue;
			}

			const nlohmann::json& engineTransform = camLockData["engine_transform"];

			if (!engineTransform.contains("position") ||
				!engineTransform.contains("scale") ||
				!engineTransform.contains("rotation"))
			{
				continue;
			}

			const nlohmann::json& posJson = engineTransform["position"];
			const nlohmann::json& scaleJson = engineTransform["scale"];
			const nlohmann::json& rotJson = engineTransform["rotation"];

			FVector3D position, rotation, scale;
			position._x = posJson.value("x", 0.f);
			position._y = posJson.value("y", 0.f);
			position._z = posJson.value("z", 0.f);

			rotation._x = rotJson.value("pitch", 0.f);
			rotation._y = rotJson.value("yaw", 0.f);
			rotation._z = rotJson.value("roll", 0.f);

			scale._x = scaleJson.value("x", 1.f);
			scale._y = scaleJson.value("y", 1.f);
			scale._z = scaleJson.value("z", 1.f);

			Ptr<CameraZone> zone = SpawnActor<CameraZone>(objectName, position, scale, rotation);
			if (!zone)
			{
				continue;
			}

			const nlohmann::json& worldLock = camLockData["camera_lock_area"];

			//const nlohmann::json& engineLock = worldLock["resolved_bounds_engine"];
			//minPoint._x = engineLock.value("x_min", 0.f);
			//minPoint._y = engineLock.value("y_min", 0.f);
			//maxPoint._x = engineLock.value("x_max", 1.f);
			//maxPoint._y = engineLock.value("y_max", 1.f);


			zone->SetTriggerSize(maxPoint._x - minPoint._x, maxPoint._y - minPoint._y);
			const nlohmann::json& sceneLock = worldLock["scene_dimensions"];
			const nlohmann::json& defaultPolicy = worldLock["default_policy"];

			_worldBound._Max._x = sceneLock.value("x_limit", 1.f) * 64.f;
			_worldBound._Max._y = sceneLock.value("y_limit", 1.f) * 64.f;

			_worldBound._Min._x = 0.f;
			_worldBound._Min._y = 0.f;

			_cameraHalfData._x = defaultPolicy.value("negative_x_min", 0.f);
			_cameraHalfData._y = defaultPolicy.value("negative_y_min", 0.f);
		}
	}

	return true;
}

Ptr<class EdgeCollisionComponent> Level::AddEdgeCollider(const std::string& name, const FVector2D& worldStart, const FVector2D& worldEnd)
{
	// CreateLevelComponent 경로(이름 finder + 매 프레임 Tick 순회)를 타지 않고 전용
	// 저장소에 담는다. 컴포넌트 id 는 유일성 유지를 위해 동일 카운터에서 발급한다.
	Ptr<EdgeCollisionComponent> edge = New<EdgeCollisionComponent>();
	const int32 compID = _levelComponentID++;
	if (!edge->Init(compID, name, This<Level>()))
	{
		DESTROY(edge);
		return nullptr;
	}

	edge->SetCollisionProfile("Environment");
	edge->SetLine(worldStart, worldEnd);

	_staticEdgeColliders.push_back(edge);
	return edge;
}

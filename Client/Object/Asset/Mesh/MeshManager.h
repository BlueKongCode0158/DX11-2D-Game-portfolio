#pragma once
#include "SubManager.h"
#include <unordered_map>
#include <filesystem>

struct MeshKey
{
	std::string AtlasName = "";
	int32 pathID = 0;

	bool operator == (const MeshKey& other) const
	{
		return AtlasName == other.AtlasName && pathID == other.pathID;
	}
};

struct MeshKeyHash
{
	size_t operator()(const MeshKey& key) const
	{
		size_t h1 = std::hash<std::string>{}(key.AtlasName);
		size_t h2 = std::hash<int32>{}(key.pathID);
		return h1 ^ (h2 << 1);
	}
};
class MeshManager : public SubManager
{
public:
	MeshManager();
	virtual ~MeshManager();
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;
private:
	std::unordered_map<std::string, Ptr<class Mesh>> _meshes;
	std::unordered_map<MeshKey, Ptr<class Mesh>, MeshKeyHash> _probMeshes;
public:
	virtual bool Init() override;
	virtual void Destroy() override;
	Ptr<class Mesh> FindMesh(const std::string& name);
	Ptr<class Mesh> FindMesh(const std::string& name, int32 pathID);
	bool CreateMesh(const std::string& name, void* vertexData, int32 size, int32 count, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive,
		void* indexData = nullptr, int32 indexSize = 0, int32 indexCount = 0, DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN, D3D11_USAGE indexUsage = D3D11_USAGE_DEFAULT);
	bool CreateSpriteMeshFromData(const std::string& meshKey, 
		int32 pathID, const void* vertexData, int32 size, int32 vertexCount, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive,
		const void* indexData = nullptr, int32 indexSize = 0, int32 indexCount = 0, DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN, D3D11_USAGE indexUsage = D3D11_USAGE_DEFAULT);
	bool LoadMesh(const std::wstring& fileName);

private:
	// JSON 파싱 / 캐시 로드 양쪽이 만들어내는 공통 중간 표현.
	struct MeshCacheEntry
	{
		std::string meshKey;
		int32 pathId = 0;
		std::vector<struct FVertexTexture> vertices;	// FVector3D pos + FVector2D uv (5 float)
		std::vector<uint16> indices;
	};

	// JSON 전체 DOM 파싱 → MeshCacheEntry 목록. (캐시 미스/무효 시에만 호출)
	bool ParseMeshJson(const std::filesystem::path& jsonPath, std::vector<MeshCacheEntry>& out);
	// 중간 표현 → 실제 Mesh 생성 (CreateSpriteMeshFromData 재사용).
	void CreateMeshesFromEntries(const std::vector<MeshCacheEntry>& entries);

	bool LoadMeshCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, std::vector<MeshCacheEntry>& out);
	bool WriteMeshCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, const std::vector<MeshCacheEntry>& in);
};


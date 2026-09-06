#include "pch.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "../../../Core/DirectoryManager.h"

#include "../../../ThirdParty/nlohmann/json.hpp"
#include "../BinaryIO.h"

namespace
{
	// 캐시 포맷 식별자 / 버전. 포맷이 바뀌면 version 을 올린다.
	constexpr uint32 kMeshCacheMagic = 'M2DM';
	constexpr uint32 kMeshCacheVersion = 1;
}

static_assert(sizeof(FVertexTexture) == 20, "FVertexTexture must be 5 contiguous floats for raw mesh cache blit");

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

Ptr<class Mesh> MeshManager::FindMesh(const std::string& name)
{
	auto it = _meshes.find(name);
	if (_meshes.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

Ptr<class Mesh> MeshManager::FindMesh(const std::string& name, int32 pathID)
{
	MeshKey key;
	key.AtlasName = name;
	key.pathID = pathID;

	auto it = _probMeshes.find(key);
	if (_probMeshes.end() == it)
	{
		return nullptr;
	}
	return it->second;
}

bool MeshManager::CreateMesh(const std::string& name, void* vertexData, int32 size, int32 count, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive, void* indexData, int32 indexSize, int32 indexCount, DXGI_FORMAT fmt, D3D11_USAGE indexUsage)
{
	Ptr<Mesh> mesh = FindMesh(name);
	if (mesh)
	{
		return false;
	}

	mesh = New<Mesh>();
	if (false == mesh->CreateMesh(vertexData, size, count, vertexUsage, primitive, indexData, indexSize, indexCount, fmt, indexUsage))
	{
		Delete(mesh);
		return false;
	}
	_meshes[name] = mesh;
	return true;
}

bool MeshManager::CreateSpriteMeshFromData(
	const std::string& meshKey,
	int32 pathID, const void* vertexData, int32 size, int32 vertexCount, D3D11_USAGE vertexUsage, D3D11_PRIMITIVE_TOPOLOGY primitive,
	const void* indexData, int32 indexSize, int32 indexCount, DXGI_FORMAT fmt, D3D11_USAGE indexUsage)
{
	std::string AtlasKey = meshKey;
	Ptr<Mesh> mesh = FindMesh(AtlasKey);
	if (mesh)
	{
		// 이미 메시가 있어? 그럼 나가
		return true;
	}

	mesh = New<Mesh>();
	if (false == mesh->CreateMesh(vertexData, size, vertexCount, vertexUsage, primitive, indexData, indexSize, indexCount, fmt, indexUsage))
	{
		return false;
	}

	MeshKey key;
	key.AtlasName = AtlasKey;
	key.pathID = pathID;

	_probMeshes[key] = mesh;

	FVertexTexture* pTextures = static_cast<FVertexTexture*>(const_cast<void*>(vertexData));
	size_t vertexBufferSize = vertexCount;

	return true;
}

bool MeshManager::LoadMesh(const std::wstring& fileName)
{
	std::filesystem::path filePath;
	std::optional<std::filesystem::path> resPath = DirectoryManager::Instance().GetCachePath("Resources");

	if (!resPath.has_value())	// 방어코드
	{
		return false;
	}

	if (!DirectoryManager::Instance().GetDirectory(resPath.value(), "Json", filePath))
	{
		return false;
	}

	if (!DirectoryManager::Instance().GetFile(filePath, fileName, filePath))
	{
		return false;
	}

	// 캐시 경로: <json>.cache.bin
	std::filesystem::path cachePath = filePath;
	cachePath.replace_extension(".cache.bin");

	std::vector<MeshCacheEntry> entries;

	// 1) 유효한 캐시가 있으면 바이너리에서 바로 로드 (JSON 파싱 없음).
	if (LoadMeshCache(cachePath, filePath, entries))
	{
		CreateMeshesFromEntries(entries);
		return true;
	}

	// 2) 캐시 미스/무효 → JSON 파싱 후 캐시 기록.
	if (!ParseMeshJson(filePath, entries))
	{
		return false;
	}

	WriteMeshCache(cachePath, filePath, entries);
	CreateMeshesFromEntries(entries);
	return true;
}

bool MeshManager::ParseMeshJson(const std::filesystem::path& jsonPath, std::vector<MeshCacheEntry>& out)
{
	std::ifstream file(jsonPath, std::ios::binary);

	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json root = nlohmann::json::parse(file, nullptr, false);

	if (root.is_discarded())
	{
		return false;
	}

	if (!root.contains("meshes") || !root["meshes"].is_object())
	{
		return false;
	}

	const nlohmann::json& meshes = root["meshes"];

	for (auto& [atlasName, meshArray] : meshes.items())
	{
		if (!meshArray.is_array())
		{
			continue;
		}

		for (const auto& meshJson : meshArray)
		{
			std::string meshName = meshJson.value("mesh_key", "");
			int32 pathId = meshJson.value("sprite_path_id", 0);

			if (meshName.empty())
			{
				continue;
			}

			if (!meshJson.contains("vertices") || !meshJson.contains("indices"))
			{
				continue;
			}

			const nlohmann::json& jsonVertices = meshJson["vertices"];
			const nlohmann::json& jsonIndices = meshJson["indices"];

			if (!jsonVertices.is_array() || !jsonIndices.is_array())
			{
				continue;
			}

			MeshCacheEntry entry;
			entry.meshKey = meshName;
			entry.pathId = pathId;
			entry.vertices.reserve(jsonVertices.size());
			entry.indices.reserve(jsonIndices.size());

			for (const auto& v : jsonVertices)
			{
				FVertexTexture vertex(
					v.value("x", 0.f),
					v.value("y", 0.f),
					v.value("z", 0.f),
					v.value("u", 0.f),
					v.value("v", 0.f));

				entry.vertices.push_back(vertex);
			}

			for (const auto& index : jsonIndices)
			{
				entry.indices.push_back(index.get<uint16>());
			}

			out.push_back(std::move(entry));
		}
	}

	return true;
}

void MeshManager::CreateMeshesFromEntries(const std::vector<MeshCacheEntry>& entries)
{
	for (const MeshCacheEntry& entry : entries)
	{
		CreateSpriteMeshFromData(entry.meshKey, entry.pathId,
			entry.vertices.data(), sizeof(FVertexTexture),
			static_cast<int32>(entry.vertices.size()),
			D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			entry.indices.data(), sizeof(uint16),
			static_cast<int32>(entry.indices.size()), DXGI_FORMAT_R16_UINT);
	}
}

bool MeshManager::LoadMeshCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, std::vector<MeshCacheEntry>& out)
{
	BinaryIO::SourceMeta srcMeta;
	if (!BinaryIO::QuerySourceMeta(jsonPath, srcMeta))
	{
		return false;
	}

	std::ifstream is(cachePath, std::ios::binary);
	if (!is.is_open())
	{
		return false;
	}

	uint32 magic = 0;
	uint32 version = 0;
	BinaryIO::SourceMeta cachedMeta;
	uint32 meshCount = 0;

	if (!BinaryIO::ReadPod(is, magic) || magic != kMeshCacheMagic ||
		!BinaryIO::ReadPod(is, version) || version != kMeshCacheVersion ||
		!BinaryIO::ReadPod(is, cachedMeta.writeTime) ||
		!BinaryIO::ReadPod(is, cachedMeta.fileSize) ||
		!BinaryIO::ReadPod(is, meshCount))
	{
		return false;
	}

	// 원본 JSON 이 바뀌었으면 캐시 무효.
	if (cachedMeta.writeTime != srcMeta.writeTime || cachedMeta.fileSize != srcMeta.fileSize)
	{
		return false;
	}

	out.clear();
	out.reserve(meshCount);

	for (uint32 i = 0; i < meshCount; ++i)
	{
		MeshCacheEntry entry;
		uint32 vertexCount = 0;
		uint32 indexCount = 0;

		if (!BinaryIO::ReadPod(is, entry.pathId) ||
			!BinaryIO::ReadStr(is, entry.meshKey) ||
			!BinaryIO::ReadPod(is, vertexCount))
		{
			return false;
		}

		// FVertexTexture 는 기본 생성자가 없으므로 fill 값을 넘겨 resize 한다.
		entry.vertices.resize(vertexCount, FVertexTexture(0.f, 0.f, 0.f, 0.f, 0.f));
		if (vertexCount > 0)
		{
			is.read(reinterpret_cast<char*>(entry.vertices.data()),
				static_cast<std::streamsize>(vertexCount) * sizeof(FVertexTexture));
		}

		if (!BinaryIO::ReadPod(is, indexCount))
		{
			return false;
		}

		entry.indices.resize(indexCount);
		if (indexCount > 0)
		{
			is.read(reinterpret_cast<char*>(entry.indices.data()),
				static_cast<std::streamsize>(indexCount) * sizeof(uint16));
		}

		if (!is)
		{
			return false;
		}

		out.push_back(std::move(entry));
	}

	return true;
}

bool MeshManager::WriteMeshCache(const std::filesystem::path& cachePath, const std::filesystem::path& jsonPath, const std::vector<MeshCacheEntry>& in)
{
	BinaryIO::SourceMeta srcMeta;
	if (!BinaryIO::QuerySourceMeta(jsonPath, srcMeta))
	{
		return false;
	}

	std::ofstream os(cachePath, std::ios::binary | std::ios::trunc);
	if (!os.is_open())
	{
		return false;
	}

	BinaryIO::WritePod(os, kMeshCacheMagic);
	BinaryIO::WritePod(os, kMeshCacheVersion);
	BinaryIO::WritePod(os, srcMeta.writeTime);
	BinaryIO::WritePod(os, srcMeta.fileSize);
	BinaryIO::WritePod(os, static_cast<uint32>(in.size()));

	for (const MeshCacheEntry& entry : in)
	{
		BinaryIO::WritePod(os, entry.pathId);
		BinaryIO::WriteStr(os, entry.meshKey);

		BinaryIO::WritePod(os, static_cast<uint32>(entry.vertices.size()));
		if (!entry.vertices.empty())
		{
			os.write(reinterpret_cast<const char*>(entry.vertices.data()),
				static_cast<std::streamsize>(entry.vertices.size()) * sizeof(FVertexTexture));
		}

		BinaryIO::WritePod(os, static_cast<uint32>(entry.indices.size()));
		if (!entry.indices.empty())
		{
			os.write(reinterpret_cast<const char*>(entry.indices.data()),
				static_cast<std::streamsize>(entry.indices.size()) * sizeof(uint16));
		}
	}

	return static_cast<bool>(os);
}

bool MeshManager::Init()
{
	FVertexColor triangle[3]
	{
		FVertexColor(0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 1.f),
		FVertexColor(0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f, 1.f),
		FVertexColor(-0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f, 1.f)
	};

	uint16 triangleIndex[3] = { 0, 1, 2 };
	if (false == CreateMesh("Triangle", triangle, sizeof(FVertexColor), 3, D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, triangleIndex, sizeof(uint16), 3, DXGI_FORMAT_R16_UINT))
	{
		return false;
	}

	// frameRect
	FVector3D frameRect[4] = {
		FVector3D(-0.5f , 0.5f, 0.f),
		FVector3D(0.5f , 0.5f, 0.f),
		FVector3D(-0.5f , -0.5f, 0.f),
		FVector3D(0.5f , -0.5f, 0.f),
	};
	uint16 frameRectIndex[5] = { 0,1,3,2,0 };
	if (!CreateMesh("frameRect", frameRect, sizeof(FVector3D), 4, D3D11_USAGE_DEFAULT, D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP, frameRectIndex, sizeof(uint16), 5, DXGI_FORMAT_R16_UINT))
	{
		return false;
	}

	// sphere
	FVector3D spherePoint[37];
	for (int i = 0; i < 37; ++i)
	{
		float angle = DirectX::XMConvertToRadians(i * 10.f);

		spherePoint[i]._x = cosf(angle) * 0.5f;
		spherePoint[i]._y = sinf(angle) * 0.5f;
	}

	if (!CreateMesh("FrameSphere", spherePoint, sizeof(FVector3D), 37, D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP))
	{
		return false;
	}

	FVertexTexture texRect[4]
	{
		FVertexTexture(-0.5f, 0.5f, 0.f, 0.f, 0.f),
		FVertexTexture(0.5f, 0.5f, 0.f, 1.f, 0.f),
		FVertexTexture(-0.5f, -0.5f, 0.f, 0.f, 1.f),
		FVertexTexture(0.5f, -0.5f, 0.f, 1.f, 1.f)
	};

	uint16 rectIndex[6] = { 0, 1, 3, 0, 3, 2 };
	if (!CreateMesh("TexRect", texRect, sizeof(FVertexTexture), 4, D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, rectIndex, sizeof(uint16), 6, DXGI_FORMAT_R16_UINT))
	{
		return false;
	}

	FVector3D Line[2]
	{
		FVector3D(-0.5f, 0.f, 0.f),
		FVector3D(0.5f, 0.f, 0.f)
	};

	if (!CreateMesh("Line", Line, sizeof(FVector3D), 2, D3D11_USAGE_DEFAULT, D3D11_PRIMITIVE_TOPOLOGY_LINELIST))
	{
		return false;
	}

	LoadMesh(L"SpriteMeshManifest.json");
	return true;
}

void MeshManager::Destroy()
{
	for (auto& it : _meshes)
	{
		if (nullptr == it.second)
		{
			continue;
		}
		DESTROY(it.second);
	}
	for (auto& it : _probMeshes)
	{
		if (nullptr == it.second)
		{
			continue;
		}
		DESTROY(it.second);
	}
	_meshes.clear();
	_probMeshes.clear();
}

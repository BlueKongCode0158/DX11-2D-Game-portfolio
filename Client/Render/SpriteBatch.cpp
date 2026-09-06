#include "pch.h"
#include "SpriteBatch.h"

#include "../Core/GameEngine.h"

#include "../Component/Animation/SpriteComponent.h"
#include "../Component/SceneComponent.h"
#include "../Object/Asset/Mesh/Mesh.h"

#include "../Object/Asset/AssetManager.h"
#include "../Shader/ShaderManager.h"
#include "../Object/Asset/Texture/Texture.h"

#include "../Core/Device.h"
#include "../Shader/ConstantBuffer/TransformCBuffer.h"

#include "../World/World.h"
#include "../World/Level.h"

#include "../Render/RenderManager.h"
#include "../Render/BlendState.h"

#include "../Component/ParallaxLayerComponent.h"

SpriteBatch::SpriteBatch()
{
}

SpriteBatch::~SpriteBatch()
{
}

bool SpriteBatch::Init(int32 initCapacity)
{
	_capacity = initCapacity;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = _capacity * sizeof(FSpriteBatchVertex);

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&desc, nullptr, _dynamicVB.GetAddressOf())))
	{
		return false;
	}

	_shader = FIND_SHADER("SpriteBatchShader");
	_transformCBuffer = FIND_CBUFFER("Transform", TransformCBuffer);
	_stagingVerts.reserve(initCapacity);
	return true;
}

void SpriteBatch::ExpandMesh(const Ptr<class SpriteComponent> comp, OUT std::vector<FSpriteBatchVertex>& out)
{
	FRenderModifier2D modifier = comp->BuildRenderModifier();

	// vertex
	const byte* vertexData = comp->GetMesh()->GetVertexRawData().data();
	int32 stride = comp->GetMesh()->GetVectexStride();

	// index
	const byte* indexData = comp->GetMesh()->GetIndexRawData().data();
	int32 indexCount = comp->GetMesh()->GetIndexCount();
	bool IsUint16 = (comp->GetMesh()->GetIndexFormat() == DXGI_FORMAT_R16_UINT);

	FMatrix world = comp->GetWorldMatrix();	
	const FVector4D& tint = comp->GetTint();

	size_t base = out.size();
	out.resize(base + indexCount);

	for (size_t i = 0; i < indexCount; i++)
	{
		uint32 indexFormat = (IsUint16 ?
			reinterpret_cast<const uint16*>(indexData)[i] :
			reinterpret_cast<const uint32*>(indexData)[i]);

		const float* vertex = reinterpret_cast<const float*>(vertexData + indexFormat * stride);

		FVector3D worldPos = FVector3D(vertex[0], vertex[1], vertex[2]);
		worldPos = worldPos.TransformCoord(world);
		if (modifier._enabled)
		{
			worldPos._x = modifier._pivot._x + (worldPos._x - modifier._pivot._x) * modifier._scale + modifier._offset._x;
			worldPos._y = modifier._pivot._y + (worldPos._y - modifier._pivot._y) * modifier._scale + modifier._offset._y;
		}
		FSpriteBatchVertex batchVertex = out[base + i];
		batchVertex._pos = worldPos;

		batchVertex._uv = FVector2D(vertex[3], vertex[4]);
		batchVertex._tint = tint;

		out[base + i] = batchVertex;
	}
}

//**포인트** :
//-`D3D11_MAP_WRITE_DISCARD`: GPU가 이전 프레임에서 VB를 읽는 도중 CPU 쓰기를 허용(드라이버가 내부 버퍼 rotate)
//- World = Identity: `ExpandMesh`에서 이미 world transform + parallax를 CPU에서 적용했으므로 VS는 view / proj만 곱함
//- `IASetIndexBuffer(nullptr)`: 인덱스 버퍼 없이 `Draw()`로 flat triangle list 전송

void SpriteBatch::Flush(const std::vector<FSpriteBatchVertex>& verts, Ptr<class Texture> tex, int32 texIndx, eBlendMode blend)
{
	if (verts.empty())
	{
		return;
	}

	int32 count = static_cast<int32>(verts.size());
	GrowIfNeeded(count);

	// 동적 VB 업로드 
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	CONTEXT->Map(_dynamicVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, verts.data(), count * sizeof(FSpriteBatchVertex));
	CONTEXT->Unmap(_dynamicVB.Get(), 0);

	// 셰이더 바인딩
	_shader->SetShader();
	ShaderManager::Instance().SetSample(eTextureSampleType::TEXTURE_SAMPLE_LINEAR);
	RenderManager::Instance().GetAlphaBlend(blend)->SetState();

	tex->SetShader(0, SHADER_TYPE::PIXEL, texIndx);
	_transformCBuffer->SetWorldMatrix(FMatrix());
	_transformCBuffer->SetViewMatrix(GameEngine::Instance().GetWorld()->GetCurLevel()->GetViewMatrix());
	_transformCBuffer->SetProjMatrix(GameEngine::Instance().GetWorld()->GetCurLevel()->GetProjMatrix());

	_transformCBuffer->Update();

	uint32 stride = sizeof(FSpriteBatchVertex);
	uint32 offset = 0;
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->IASetVertexBuffers(0, 1, _dynamicVB.GetAddressOf(), &stride, &offset);
	CONTEXT->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, offset);
	CONTEXT->Draw(count, 0);

	RenderManager::Instance().GetAlphaBlend(blend)->ResetState();
}

void SpriteBatch::Destroy()
{
}

void SpriteBatch::GrowIfNeeded(int32 required)
{
	if (required <= _capacity)
	{
		return;
	}

	while (_capacity < required)
	{
		_capacity *= 2;
	}

	_dynamicVB.Reset();
	D3D11_BUFFER_DESC desc = {};

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = _capacity * sizeof(FSpriteBatchVertex);

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&desc, nullptr, _dynamicVB.GetAddressOf())))
	{
		return;
	}
	return;
}

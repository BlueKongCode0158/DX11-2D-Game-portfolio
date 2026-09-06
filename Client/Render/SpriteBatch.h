#pragma once

#include "../Object/Object.h"

struct FSpriteBatchVertex
{
    FVector3D _pos;
    FVector2D _uv;
    FVector4D _tint;
};

struct SpriteBatchKey
{
    Ptr<class Texture> _texture;
    int32              _textureIndex;
    eBlendMode         _blend;
    bool operator==(const SpriteBatchKey& other) const
    {
        return !_texture.owner_before(other._texture)
            && !other._texture.owner_before(_texture)
            && _textureIndex == other._textureIndex
            && _blend == other._blend;
    }
};

class SpriteBatch : public Object
{
public:
    SpriteBatch();
    virtual ~SpriteBatch();
private:
    ComPtr<ID3D11Buffer> _dynamicVB;
    Ptr<class Shader> _shader;
    Ptr<class TransformCBuffer> _transformCBuffer;
    std::vector<FSpriteBatchVertex> _stagingVerts;
    int32 _capacity = 0;
public:
    bool Init(int32 initCapacity = 8192);
    void ExpandMesh(const Ptr<class SpriteComponent> comp, OUT std::vector<FSpriteBatchVertex>& out);
    void Flush(const std::vector<FSpriteBatchVertex>& verts,
        Ptr<class Texture> tex, int32 texIndx,
        eBlendMode blend);
    virtual void Destroy() override;
public:
    std::vector<FSpriteBatchVertex>& GetStagingVerts()
    {
        return _stagingVerts;
    }
private:
    void GrowIfNeeded(int32 required);
};
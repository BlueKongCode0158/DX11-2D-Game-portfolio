#include "Share.fx"

cbuffer TransformCBuffer : register(b0)
{
    matrix matWorld;
    matrix matView;
    matrix matProj;
    matrix matWV;
    matrix matWVP;
};

cbuffer ParticleCBuffer : register(b1)
{
    int     gUseTexture;
    float3  gParticleEmpty;
};

/*
struct FParticleInstance
{
	FVector3D _position;
	float	  _size;
	FVector4D _color;
	float	  _rotation;
};
*/

struct FParticleData
{
    float3  gPosition;
    float   gSize;
    float4  gColor;
    float   gRotation;
    float2  gUVLT;
    float2  gUVRB;
};

StructuredBuffer<FParticleData> gParticles : register(t0);

struct VS_INPUT
{
    float3   position    : POSITION;
    float2   uv          : TEXCOORD;
    uint     instID      : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD;
    float4 color    : COLOR;
};

VS_OUTPUT ParticleInstanceVS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    FParticleData data = gParticles[input.instID];
    
    float2 localPos = input.position.xy * data.gSize;
    float rotCos = cos(data.gRotation);
    float rotSin = sin(data.gRotation);
    float2 rotated;
    
    rotated.x = localPos.x * rotCos - localPos.y * rotSin;
    rotated.y = localPos.x * rotSin + localPos.y * rotCos;
    
    float3 worldPos = float3(rotated + data.gPosition.xy, data.gPosition.z);
    float4 world = mul(float4(worldPos, 1.f), matWorld);
    float4 view = mul(world, matView);
    output.position = mul(view, matProj);
    
    output.uv = input.uv;
    output.color = data.gColor;
    
    return output;
}

PS_Output_Single ParticleInstancePS(VS_OUTPUT input) : SV_Target
{
    PS_Output_Single output = (PS_Output_Single) 0;
    output.Color = input.color;
    
    if(gUseTexture)
    {
        float4 tex = gBaseTexture.Sample(gBaseSample, input.uv);
        output.Color *= tex;
    }
    return output;
}
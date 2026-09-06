#include "Share.fx"

struct VS_Input_Batch
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float4 Tint : COLOR;
};

struct VS_Output_Batch
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float4 Tint : COLOR;
};

VS_Output_Batch SpriteBatchVS(VS_Input_Batch Input)
{
    VS_Output_Batch output = (VS_Output_Batch) 0;
    
    float4 worldPos = float4(Input.Pos, 1.f);
    
    output.Pos = mul(mul(worldPos, gView), gProj);
    output.UV = Input.UV;
    output.Tint = Input.Tint;
    
    return output;
}

PS_Output_Single SpriteBatchPS(VS_Output_Batch Input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    float4 color = gBaseTexture.Sample(gBaseSample, Input.UV);
    color *= Input.Tint;
    
    output.Color = color;
    return output;
}
#include "Share.fx"

struct VS_Input_Color
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct VS_Output_Color
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_Output_Color ColorMeshVS(VS_Input_Color input)
{
    VS_Output_Color output = (VS_Output_Color) 0;
    
    output.Pos = mul(float4(input.Pos, 1.f), gWVP);
    output.Color = input.Color;
    
    return output;
}

PS_Output_Single ColorMeshPS(VS_Output_Color input)
{   
    PS_Output_Single output = (PS_Output_Single) 0;
    output.Color = input.Color;
    return output;
}

float4 FrameMeshVS(float3 Pos : POSITION) : SV_POSITION
{
    float4 output = (float4) 0;
    float3 InputPos = Pos;
    output = mul(float4(InputPos, 1.f), gWVP);
    return output;
}

PS_Output_Single FrameMeshPS(float4 Pos : SV_POSITION)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    output.Color = gColor;
    return output;
}
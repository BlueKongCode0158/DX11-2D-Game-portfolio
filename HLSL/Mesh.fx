#include "Share.fx"

struct VS_Input_Tex
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VS_Ouput_Tex
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};


VS_Ouput_Tex MaterialMeshVS(VS_Input_Tex input)
{
    VS_Ouput_Tex output = (VS_Ouput_Tex) 0;
    
    output.Pos = mul(float4(input.Pos, 1.f), gWVP);
    output.UV = input.UV;
    
    return output;
}

PS_Output_Single DefaultMaterialPS(VS_Ouput_Tex input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    
    float4 color = gBaseTexture.Sample(gBaseSample, input.UV);
    
    color.rgb *= gMtrlBaseColor.rgb;
    color.a *= gMtrOpacity;
    
    output.Color = color;
    return output;
}
#include "Share.fx"

struct VS_Input_Tex
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VS_Output_Tex
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

cbuffer Sprite : register(b2)
{
    float4 gSpriteTint; // 베이스 컬러
    float2 gParallaxOffset;
    float2 gParallaxPivot;
    float  gParallaxScale;
    float3 gSpriteEmpty;
}

VS_Output_Tex SpriteVS(VS_Input_Tex input)
{
    VS_Output_Tex output = (VS_Output_Tex) 0;
    float3 pos = (float3)0;
    pos = input.Pos;
    
    if(gAnim2DFlip  == 1)
    {
        pos.x = -input.Pos.x;
    }
    
    float4 worldPos = mul(float4(pos, 1.f), gWorld);
    worldPos.xy = gParallaxPivot + (worldPos.xy - gParallaxPivot) * gParallaxScale;
    worldPos.xy += gParallaxOffset.xy;
    
    output.Pos = mul(mul(worldPos, gView), gProj);
    output.UV = UpdateAnimation2D(input.UV);
    return output;
}

PS_Output_Single SpritePS(VS_Output_Tex input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    float4 color = gBaseTexture.Sample(gBaseSample, input.UV);
    output.Color = color * gSpriteTint;

    return output;
}

VS_Output_Tex SpriteMeshVS(VS_Input_Tex input)
{
    VS_Output_Tex output = (VS_Output_Tex) 0;

    float4 worldPos = mul(float4(input.Pos, 1.f), gWorld);
    worldPos.xy = gParallaxPivot + (worldPos.xy - gParallaxPivot) * gParallaxScale;
    worldPos.xy += gParallaxOffset.xy;
    
    output.Pos = mul(mul(worldPos, gView), gProj);
    output.UV = input.UV;

    return output;
}

PS_Output_Single SpriteMeshPS(VS_Output_Tex input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    float4 color = gBaseTexture.Sample(gBaseSample, input.UV);

    color = color * gSpriteTint;
    color.rgb *= color.a * gSpriteTint.a;
    output.Color = color;
    output.Color.a = color.a;
    return output;
}

VS_Output_Tex FogVS(VS_Input_Tex input)
{
    VS_Output_Tex output = (VS_Output_Tex) 0;
    
    output.Pos = mul(float4(input.Pos, 1.f), gWVP);
    output.UV = input.UV;

    return output;
}

PS_Output_Single FogPS(VS_Output_Tex input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    float4 tex = gBaseTexture.Sample(gBaseSample, input.UV);

    // 텍스처 알파 그대로 사용
    float alpha = tex.a * 0.25f;

    float3 cloudColor = float3(0.65f, 0.72f, 0.82f);

    output.Color.rgb = cloudColor * alpha;
    output.Color.a = alpha;
    return output;
}
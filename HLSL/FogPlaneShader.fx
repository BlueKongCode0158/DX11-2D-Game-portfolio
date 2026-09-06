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
    float4 ScreenPos : TEXCOORD1;
};

//원인:
//float2 뒤에 float4가 오면
//HLSL이 자동 패딩 삽입

//float4는16 바이트
//경계에만 배치될 수 있어서, float2(8바이트)
//다음에 오면8 바이트 패딩이 강제로 생깁니다.

cbuffer FogPlane : register(b3)
{
    float4 gTint;
    float2 gScreenSize;
    float gTime;
    float gDistortion;
}

Texture2D gBackgroundTex : register(t0);
Texture2D gNoiseTexture : register(t1);

VS_Output_Tex FogPlaneVS(VS_Input_Tex input)
{
    VS_Output_Tex output = (VS_Output_Tex) 0;
    output.Pos = mul(float4(input.Pos, 1.f), gWVP);
    output.UV = input.UV;
    output.ScreenPos = output.Pos;
    return output;
}

float4 BlurGrabTexture(float2 uv)
{
    float2 texel = 1.f / gScreenSize;
    float radius = 12.f;
    
    float4 col = 0.f;
    float weight = 0.f;

    float blue = 1.f;
    
    
    [unroll]
    for (int y = -blue; y <= blue; ++y)
    {
        [unroll]
        for (int x = -blue; x <= blue; ++x)
        {
            float w = 1.f;
            if (abs(x) + abs(y) == 0)
                w = 4.f;
            else if (abs(x) + abs(y) == 1)
                w = 2.f;

            float2 sampleUV = uv + float2(x, y) * texel * radius;
            sampleUV = clamp(sampleUV, texel * 0.5f, 1.f - texel * 0.5f);
            
            col += gBackgroundTex.Sample(gBaseSample, sampleUV) * w;
            weight += w;
        }
    }

    return col / weight;
}

PS_Output_Single FogPlanePS(VS_Output_Tex input)
{
    PS_Output_Single output = (PS_Output_Single) 0;
    
    float2 ndc = input.ScreenPos.xy / input.ScreenPos.w;
    float2 screenUV = float2(ndc.x * 0.5 + 0.5, -ndc.y * 0.5 + 0.5);
    
    float2 noiseUV1 = input.UV * 0.5f + float2(gTime * 0.005f, gTime * 0.002f);
    float2 noiseUV2 = input.UV * 0.3f - float2(gTime * 0.001f, gTime * 0.008f);
   
    float2 noise = (gNoiseTexture.Sample(gBaseSample, noiseUV1).rg +
                    gNoiseTexture.Sample(gBaseSample, noiseUV2).rg) * 0.5f;
    
    noise = noise * 2.f - 1.f;
    
    float2 texel = 1.f / gScreenSize;
    float2 distortedUV = screenUV + noise * gDistortion * texel;
    distortedUV = clamp(distortedUV, texel * 2.f, 1.f - texel * 2.f);
    //distortedUV = saturate(distortedUV);
    
    float4 col = BlurGrabTexture(distortedUV);
    
    float2 uv = input.UV;
    float edgeFade = min(min(uv.x, 1.f - uv.x), min(uv.y, 1.f - uv.y));
    edgeFade = saturate(edgeFade * 7.f);
    
    float alpha = 0.35f * edgeFade * gTint.rgb;
    float3 rgb = col.rgb * gTint.rgb;
    
    output.Color = float4(rgb, alpha);
    return output;
}

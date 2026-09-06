struct VSOut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
};

cbuffer CompositeDebug : register(b5)
{
    int     gShowScene;
    int     gShowLight;
    float2  gEmpty;
}

Texture2D gSceneTexture : register(t0);
Texture2D gLightTexture : register(t1);
SamplerState gBaseSample : register(s0);

VSOut LightCompositeVS(uint id : SV_VertexID)
{
    // Fullscreen triangle
    float2 pos[3] =
    {
        float2(-1.f, -1.f),
        float2(-1.f, 3.f),
        float2(3.f, -1.f)
    };
    
    float2 uv[3] =
    {
        float2(0.f, 1.f),
        float2(0.f, -1.f),
        float2(2.f, 1.f)
    };
    
    VSOut output;
    output.Pos = float4(pos[id], 0.f, 1.f);
    output.UV = uv[id];
    return output;
}


float4 LightCompositePS(VSOut input) : SV_Target
{
    float4 scene = gSceneTexture.Sample(gBaseSample, input.UV);
    float4 light = gLightTexture.Sample(gBaseSample, input.UV);

#ifdef _EDITOR
    if(0 == gShowScene && 0 == gShowLight)
    {
        return float4(0.f, 0.f, 0.f, scene.a);
    }
    else if(1 == gShowScene && 0 == gShowLight)
    {
        return scene;
    }
    else if(0 == gShowScene && 1 == gShowLight)
    {
        return float4(light.rgb, 1.f);
    }
#endif
    
    float3 sceneColor = scene.a > 1e-5 ? scene.rgb / scene.a : scene.rgb;
    
    float ambient = 1.f;
    float exposure = 1.2f;
    float glowPower = 0.7f;

    float3 lightSoft = 1.f - exp(-light.rgb * exposure);
    float strength = saturate(max(lightSoft.r, max(lightSoft.g, lightSoft.b)));

    // 빛 없는 곳은 어둡게, 빛 있는 곳은 원본 밝기까지 복구
    float3 darkScene = scene.rgb * ambient;
    float3 baseLit = lerp(darkScene, scene.rgb, strength);

    // beam/white light 자체를 추가
    float3 glow = lightSoft * glowPower;

    float3 result = baseLit + glow;
     
    return float4(result, scene.a);
    //return light;
}
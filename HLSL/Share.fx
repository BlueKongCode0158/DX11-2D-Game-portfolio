struct PS_Output_Single
{
    float4 Color : SV_TARGET;
};

cbuffer Transform : register(b0)
{
    matrix gWorld;
    matrix gView;
    matrix gProj;
    matrix gWV;
    matrix gWVP;
}

cbuffer Color : register(b0)
{
    float4 gColor;
}

cbuffer Material : register(b1)
{
    float4 gMtrlBaseColor;
    float gMtrOpacity;
    int gMtrTextureWidth;
    int gMtrTextureHeight;
    float gEmpty;
}

cbuffer Animation2D : register(b1)
{
    float2 gAnim2DLTUV;
    float2 gAnim2DRBUV;
    float2 gAnim2DOffset;
    int gAnim2DFlip;
    int gAnim2DRotated90;
}

SamplerState gBaseSample : register(s0);
Texture2D gBaseTexture : register(t0);

float2 UpdateAnimation2D(float2 UV)
{
    float2 result = (float2) 0.f;
    float2 uv = UV; 
    if (gAnim2DRotated90)
    {
        uv.x = 1.f - uv.x;
        uv = float2(1.f - uv.y, uv.x);
    }
    
    result.x = lerp(gAnim2DLTUV.x, gAnim2DRBUV.x, uv.x);
    result.y = lerp(gAnim2DLTUV.y, gAnim2DRBUV.y, uv.y);
    return result;
}

float2 GetAnimation2DOffset()
{
    float2 offset = gAnim2DOffset;
    if(gAnim2DFlip ==  1)
    {
        offset.x = -offset.x;
    }
    
    return offset;
}

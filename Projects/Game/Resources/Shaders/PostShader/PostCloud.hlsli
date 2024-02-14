Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer Matrix : register(b0){
    float4x4 kWorldmat;
    float4x4 kViewProjectionMatrix;
}

cbuffer Color : register(b1){
    float4 kColor;
}

cbuffer MoveVector : register(b2)
{
    float2 kMoveVec;
}

cbuffer DensityScale :register(b3) {
    float kDensityScale;
    float kExponent;
    float kColorTime;
}

cbuffer SkyColor : register(b4)
{
    float4 kSkyColor;
}

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 worldPos : POSITION;
};
Texture2D<float4> tex : register(t0);
Texture2D<float4> caustics : register(t1);
SamplerState smp : register(s0);

cbuffer Matrix : register(b0){
    float4x4 worldmat;
    float4x4 viewProjectionMatrix;
}

cbuffer Color : register(b1){
    float4 color;
}

cbuffer NormalVector : register(b2)
{
    float3 kNormal;
    float3 kTangent;
}

cbuffer DensityScale :register(b5) {
    float kDensityScale;
}

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 causticsUv : TEXCOORD1;
    float4 worldPos : POSITION;
    float3x3 tangentBasis : YANGENT_BASIS;
};
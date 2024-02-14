Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
StructuredBuffer<float4x4> mat : register(t1);
StructuredBuffer<float4> color : register(t2);

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 pxcolor : COLOR;
};
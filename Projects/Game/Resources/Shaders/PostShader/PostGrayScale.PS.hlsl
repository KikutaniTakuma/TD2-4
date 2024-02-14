#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float4 color;

    color = tex.Sample(smp, input.uv);
    
    float t = dot(color.xyz, float3(0.299f, 0.587f, 0.144f));

    clip(t - 0.9f);

    return color;
}
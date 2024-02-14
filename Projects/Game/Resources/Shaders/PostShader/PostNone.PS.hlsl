#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float4 finalColor =  tex.Sample(smp, input.uv);

    return finalColor;
}
#include "Texture2D.hlsli"

float4 main(Output input) : SV_TARGET{
    return tex.Sample(smp, input.uv) * color;
}
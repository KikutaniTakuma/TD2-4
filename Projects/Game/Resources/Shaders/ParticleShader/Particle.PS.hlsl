#include "Particle.hlsli"

float4 main(Output input) : SV_TARGET{
    return tex.Sample(smp, input.uv) * input.pxcolor;
}
#include "Texture2D.hlsli"

float4 main(Output input) : SV_TARGET{
    uint32_t texIndex = textureIndex[input.instanceId];


    return tex[texIndex].Sample(smp, input.uv) * color[input.instanceId];
}
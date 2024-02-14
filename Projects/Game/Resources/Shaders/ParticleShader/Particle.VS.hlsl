#include "Particle.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD, uint32_t instanceId : SV_InstanceID){
    Output output;

    output.svPos = mul(pos, mat[instanceId]);
    output.uv = uv;
    output.pxcolor = color[instanceId];

    return output;
}
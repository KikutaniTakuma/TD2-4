#include "PostCloud.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;
    pos.w = 1.0f;

    output.worldPos = mul(pos, kWorldmat);
    output.svPos = mul(output.worldPos, kViewProjectionMatrix);
    output.uv = uv;

    return output;
}
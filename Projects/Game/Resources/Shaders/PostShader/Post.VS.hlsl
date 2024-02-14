#include "Post.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;

    output.svPos = mul(pos, worldmat);
    output.svPos = mul(output.svPos, viewProjectionMatrix);
    output.uv = uv;

    return output;
}
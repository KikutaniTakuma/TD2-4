#include "PostWater.hlsli"

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;
    pos.w = 1.0f;

    output.worldPos = mul(pos, worldmat);
    output.svPos = mul(output.worldPos, viewProjectionMatrix);
    output.uv = uv;
    output.causticsUv = uv * 10.0f * kDensityScale;
    
    float3 N = normalize(mul(kNormal, (float3x3) worldmat));
    float3 T = normalize(mul(kTangent, (float3x3) worldmat));
    float3 B = normalize(cross(N, T));
    
    output.tangentBasis = transpose(float3x3(T, B, N));

    return output;
}
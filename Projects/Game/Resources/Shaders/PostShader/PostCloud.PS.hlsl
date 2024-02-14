#include "PostCloud.hlsli"
#include "PerlinNoise.hlsli"

float4 main(Output input) : SV_TARGET{
    float perlinNoise = FractalSumNnoise(kDensityScale, input.uv + kMoveVec);
    float perlinNoise2 = FractalSumNnoise(kDensityScale, input.uv + (kMoveVec * 0.001f));

    float4 skyColor = tex.Sample(smp, input.uv) * kSkyColor;
    float4 cloudColor = tex.Sample(smp, input.uv+ kMoveVec);
    cloudColor *= pow(lerp(perlinNoise,perlinNoise2, 0.5f),kExponent) * kColorTime;

    float4 finalColor = cloudColor + skyColor;
    finalColor *= kColor;

    return finalColor;
}
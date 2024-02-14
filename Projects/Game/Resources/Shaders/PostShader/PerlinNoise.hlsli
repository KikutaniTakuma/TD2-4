#include "Random.hlsli"

float Perlin(float density, float2 uv)
{
    float2 uvFloor = floor(uv * density);
    float2 uvFrac = frac(uv * density);

    float2 v00 = RandomVector(uvFloor + float2(0.0f, 0.0f));
    float2 v01 = RandomVector(uvFloor + float2(0.0f, 1.0f));
    float2 v10 = RandomVector(uvFloor + float2(1.0f, 0.0f));
    float2 v11 = RandomVector(uvFloor + float2(1.0f, 1.0f));

    float c00 = dot(v00, uvFrac - float2(0.0f, 0.0f));
    float c01 = dot(v01, uvFrac - float2(0.0f, 1.0f));
    float c10 = dot(v10, uvFrac - float2(1.0f, 0.0f));
    float c11 = dot(v11, uvFrac - float2(1.0f, 1.0f));

    float2 u = uvFrac * uvFrac * (3.0f - 2.0f * uvFrac);

    float v0010 = lerp(c00, c10, u.x);
    float v0111 = lerp(c01, c11, u.x);

    float n = (lerp(v0010, v0111, u.y) * 0.5f) + 0.5f;
    
    return n;
}

float FractalSumNnoise(float density, float2 uv)
{
    float fn = 0.0f;
    fn += Perlin(density * 1.0f, uv) * 1.0f / 2.0f;
    fn += Perlin(density * 2.0f, uv) * 1.0f / 4.0f;
    fn += Perlin(density * 4.0f, uv) * 1.0f / 8.0f;
    fn += Perlin(density * 8.0f, uv) * 1.0f / 16.0f;

    return fn;
}
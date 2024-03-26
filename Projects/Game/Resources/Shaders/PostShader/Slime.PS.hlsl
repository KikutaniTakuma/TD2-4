#include "Post.hlsli"
#include "PerlinNoise.hlsli"

struct Vector2{
    float32_t vec;
};

ConstantBuffer<Vector2> kRandomVec : register(b2);

float CreateNoiseNoDdy(float2 uv)
{
    float density = 20.0f;
    
    float pn = FractalSumNnoise(density * 2.0f, uv + kRandomVec.vec);
    float pn2 = FractalSumNnoise(density, uv - kRandomVec.vec);
    uv.x *= -1.0f;
    float pn3 = FractalSumNnoise(density * 2.0f, uv + kRandomVec.vec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float pn4 = FractalSumNnoise(density, uv + kRandomVec.vec);
    
    float noise = lerp((pn * 0.1f), (pn2 * 0.08f), 3.0f);
    float noise2 = lerp((pn3 * 0.1f), (pn4 * 0.08f), 3.0f);
    
    return lerp(noise, noise2, 0.5f);
}


float4 main(Output input) : SV_TARGET{
    float noise = CreateNoiseNoDdy(input.uv * 0.1f);
    
    float2 uv = input.uv.xy;
    uv.x += noise * 0.1f;
    uv.y += noise;

    float4 texColor = tex.Sample(smp, uv);

    float4 finalColor = texColor;
    
    finalColor *= color;

    return finalColor;
}
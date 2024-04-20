#include "Random.hlsli"

float32_t Perlin(float32_t density, float32_t2 uv)
{
    float32_t2 uvFloor = floor(uv * density);
    float32_t2 uvFrac = frac(uv * density);

    float32_t2 v00 = RandomVector(uvFloor + float32_t2(0.0f, 0.0f));
    float32_t2 v01 = RandomVector(uvFloor + float32_t2(0.0f, 1.0f));
    float32_t2 v10 = RandomVector(uvFloor + float32_t2(1.0f, 0.0f));
    float32_t2 v11 = RandomVector(uvFloor + float32_t2(1.0f, 1.0f));

    float32_t c00 = dot(v00, uvFrac - float32_t2(0.0f, 0.0f));
    float32_t c01 = dot(v01, uvFrac - float32_t2(0.0f, 1.0f));
    float32_t c10 = dot(v10, uvFrac - float32_t2(1.0f, 0.0f));
    float32_t c11 = dot(v11, uvFrac - float32_t2(1.0f, 1.0f));

    float32_t2 u = uvFrac * uvFrac * (3.0f - 2.0f * uvFrac);

    float32_t v0010 = lerp(c00, c10, u.x);
    float32_t v0111 = lerp(c01, c11, u.x);

    float32_t n = (lerp(v0010, v0111, u.y) * 0.5f) + 0.5f;
    
    return n;
}

float32_t FractalSumNnoise(float32_t density, float32_t2 uv)
{
    float32_t fn = 0.0f;
    fn += Perlin(density * 1.0f, uv) * 1.0f / 2.0f;
    fn += Perlin(density * 2.0f, uv) * 1.0f / 4.0f;
    fn += Perlin(density * 4.0f, uv) * 1.0f / 8.0f;
    fn += Perlin(density * 8.0f, uv) * 1.0f / 16.0f;

    return fn;
}

float32_t CreateNoise(float32_t2 uv, float32_t2 vec, float32_t densityScale)
{
    float32_t density = 20.0f * densityScale;
    
    float32_t pn = FractalSumNnoise(density, uv + vec);
    float32_t pn2 = FractalSumNnoise(density, uv - vec);
    uv.x *= -1.0f;
    float32_t pn3 = FractalSumNnoise(density, uv + vec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float32_t pn4 = FractalSumNnoise(density, uv + vec);
    
    float32_t noise = lerp((pn * 0.1f), (pn2 * 0.1f), 3.0f);
    float32_t noise2 = lerp((pn3 * 0.1f), (pn4 * 0.1f), 3.0f);
    
    return ddy(noise + noise2);
}

float32_t CreateNoiseNoDdy(float32_t2 uv, float32_t2 vec, float32_t densityScale)
{
    float32_t density = 20.0f * densityScale;
    
    float32_t pn = FractalSumNnoise(density * 2.0f, uv + vec);
    float32_t pn2 = FractalSumNnoise(density, uv - vec);
    uv.x *= -1.0f;
    float32_t pn3 = FractalSumNnoise(density * 2.0f, uv + vec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float32_t pn4 = FractalSumNnoise(density, uv + vec);
    
    float32_t noise = lerp((pn * 0.1f), (pn2 * 0.08f), 3.0f);
    float32_t noise2 = lerp((pn3 * 0.1f), (pn4 * 0.08f), 3.0f);
    
    return lerp(noise, noise2, 0.5f);
}

float32_t3 CreateNormal(float32_t2 uv, float32_t2 vec, float32_t densityScale)
{
    float32_t heightScale = 5.0f;
    
    float32_t right = CreateNoise(float32_t2(uv.x + 1.0f, uv.y), vec, densityScale) * heightScale;
    float32_t left = CreateNoise(float32_t2(uv.x - 1.0f, uv.y), vec, densityScale) * heightScale;
    float32_t up = CreateNoise(float32_t2(uv.x, uv.y + 1.0f), vec, densityScale) * heightScale;
    float32_t bottom = CreateNoise(float32_t2(uv.x, uv.y - 1.0f), vec, densityScale) * heightScale;
    
    float32_t dfx = right - left;
    float32_t dfy = up - bottom;
    
    float32_t3 n = normalize(float32_t3(-dfx, -dfy, 2.0f));
    
    float32_t a = 0.01f;
    n = (n / a) * 0.5f;
    
    n = (n * 2.0f) - 1.0f;
   
    return normalize(n);
}
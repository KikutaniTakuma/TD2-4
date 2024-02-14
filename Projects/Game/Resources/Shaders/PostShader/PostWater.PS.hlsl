#include "PostWater.hlsli"
#include "PerlinNoise.hlsli"

cbuffer RamdomVec : register(b3)
{
    float2 randomVec;
}

struct Light
{
    float3 ligDirection;
    float3 ligColor;
    float3 eyePos;

    float3 ptPos;
    float3 ptColor;
    float ptRange;
};

ConstantBuffer<Light> light : register(b4);

float CreateNoise(float2 uv)
{
    float density = 20.0f * kDensityScale;
    
    float pn = FractalSumNnoise(density, uv + randomVec);
    float pn2 = FractalSumNnoise(density, uv - randomVec);
    uv.x *= -1.0f;
    float pn3 = FractalSumNnoise(density, uv + randomVec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float pn4 = FractalSumNnoise(density, uv + randomVec);
    
    float noise = lerp((pn * 0.1f), (pn2 * 0.1f), 3.0f);
    float noise2 = lerp((pn3 * 0.1f), (pn4 * 0.1f), 3.0f);
    
    return ddy(noise + noise2);
}

float CreateNoiseNoDdy(float2 uv)
{
    float density = 20.0f * kDensityScale;
    
    float pn = FractalSumNnoise(density * 2.0f, uv + randomVec);
    float pn2 = FractalSumNnoise(density, uv - randomVec);
    uv.x *= -1.0f;
    float pn3 = FractalSumNnoise(density * 2.0f, uv + randomVec);
    uv.x *= -1.0f;
    uv.y *= -1.0f;
    float pn4 = FractalSumNnoise(density, uv + randomVec);
    
    float noise = lerp((pn * 0.1f), (pn2 * 0.08f), 3.0f);
    float noise2 = lerp((pn3 * 0.1f), (pn4 * 0.08f), 3.0f);
    
    return lerp(noise, noise2, 0.5f);
}

float3 CreateNormal(float2 uv)
{
    float heightScale = 5.0f;
    
    float right = CreateNoise(float2(uv.x + 1.0f, uv.y)) * heightScale;
    float left = CreateNoise(float2(uv.x - 1.0f, uv.y)) * heightScale;
    float up = CreateNoise(float2(uv.x, uv.y + 1.0f)) * heightScale;
    float bottom = CreateNoise(float2(uv.x, uv.y - 1.0f)) * heightScale;
    
    float dfx = right - left;
    float dfy = up - bottom;
    
    float3 n = normalize(float3(-dfx, -dfy, 2.0f));
    
    float a = 0.01f;
    n = (n / a) * 0.5f;
    
    n = (n * 2.0f) - 1.0f;
   
    return normalize(n);
}

float4 main(Output input) : SV_TARGET{
    float noise = CreateNoise(input.uv);
    
    float4 texColor = tex.Sample(smp, input.uv.xy + (CreateNoiseNoDdy(input.uv)));
    
    float4 causticsColor = caustics.Sample(smp, input.causticsUv.xy + frac(CreateNoiseNoDdy(input.causticsUv * 0.1f)));
    
    float3 normal = CreateNormal(input.uv);
    //normal = mul(normal, input.tangentBasis);
    //normal = (normal.xyz + 1.0f) * 0.5f;

    float3 ligDirection = light.ligDirection;
    ligDirection = mul(ligDirection, input.tangentBasis);
    
    // ディレクションライト拡散反射光
    float t = dot(normal, -ligDirection);

    //t *= -1.0f;
    //t = (t + abs(t)) * 0.5f;
    
    t = saturate(t);
    //t = pow(t, 2.0f);

    float3 diffDirection = light.ligColor * t * 1.0f;
    
    
    float3 toEye = light.eyePos - input.worldPos.xyz;
    toEye = mul(toEye, input.tangentBasis);
    toEye = normalize(toEye);
    
    float3 refVec = -reflect(toEye, normal);
    refVec = normalize(refVec);

    t = dot(refVec, toEye);

    t = pow(saturate(t), 25.0f);
    float3 specDirection = light.ligColor * t;
    
    float3 lig = diffDirection + specDirection;
    //float3 lig = specPerlin;
    
    lig.xyz += 0.3f;
    
    //lig = pow(lig, 1.0f);
    
    float4 finalColor = texColor + causticsColor;
    
    finalColor *= color;
    finalColor.xyz *= lig;

    return finalColor;
    //return float4(normal, 1.0f);
    
    //return float4(float3(noise, noise, noise) * 2.0f, 1.0f);
}
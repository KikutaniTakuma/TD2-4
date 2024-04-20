#include "PostWater.hlsli"
#include "../PerlinNoise.hlsli"

struct RamdomVec
{
    float32_t2 randomVec;
}

ConstantBuffer<RandomVec> kRandomVec : register(b3);

struct Light
{
    float32_t3 ligDirection;
    float32_t3 ligColor;
    float32_t3 eyePos;

    float32_t3 ptPos;
    float32_t3 ptColor;
    float32_t ptRange;
};

ConstantBuffer<Light> light : register(b4);

float32_t4 main(Output input) : SV_TARGET{
    float32_t noise = CreateNoise(input.uv, kRandomVec.randomVec);
    
    float32_t4 texColor = tex.Sample(smp, input.uv.xy + (CreateNoiseNoDdy(input.uv, kRandomVec.randomVec)));
    
    float32_t4 causticsColor = caustics.Sample(smp, input.causticsUv.xy + frac(CreateNoiseNoDdy(input.causticsUv * 0.1f, kRandomVec.randomVec)));
    
    float32_t3 normal = CreateNormal(input.uv);
    //normal = mul(normal, input.tangentBasis);
    //normal = (normal.xyz + 1.0f) * 0.5f;

    float32_t3 ligDirection = light.ligDirection;
    ligDirection = mul(ligDirection, input.tangentBasis);
    
    // ディレクションライト拡散反射光
    float32_t t = dot(normal, -ligDirection);

    //t *= -1.0f;
    //t = (t + abs(t)) * 0.5f;
    
    t = saturate(t);
    //t = pow(t, 2.0f);

    float32_t3 diffDirection = light.ligColor * t * 1.0f;
    
    
    float32_t3 toEye = light.eyePos - input.worldPos.xyz;
    toEye = mul(toEye, input.tangentBasis);
    toEye = normalize(toEye);
    
    float32_t3 refVec = -reflect(toEye, normal);
    refVec = normalize(refVec);

    t = dot(refVec, toEye);

    t = pow(saturate(t), 25.0f);
    float32_t3 specDirection = light.ligColor * t;
    
    float32_t3 lig = diffDirection + specDirection;
    //float32_t3 lig = specPerlin;
    
    lig.xyz += 0.3f;
    
    //lig = pow(lig, 1.0f);
    
    float32_t4 finalColor = texColor + causticsColor;
    
    finalColor *= color;
    finalColor.xyz *= lig;

    return finalColor;
    //return float32_t4(normal, 1.0f);
    
    //return float32_t4(float32_t3(noise, noise, noise) * 2.0f, 1.0f);
}
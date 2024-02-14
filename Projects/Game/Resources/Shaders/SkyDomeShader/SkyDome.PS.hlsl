#include "SkyDome.hlsli"
#define M_PI 3.1415926535897932384626433832795028

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;
	input.normal *= -1.0f;
	
	output.color = tex.Sample(smp, input.uv);

	/*float rayleighCoefficient = 8.0f * 
		pow(M_PI, 3.0f) * 
		pow(pow(kRayleighScattering.air.refractiveIndex, 2.0f) - 1.0f, 2.0f) 
		/ (3.0f * kRayleighScattering.air.moleculesNum * pow(kRayleighScattering.air.wavelength, 4.0f));

	float theta = dot(kRayleighScattering.light.direction, kRayleighScattering.viewDirection);

	float rayleighScatter = rayleighCoefficient * (1.0f + pow(theta, 2.0f));

	float3 scatterColor = kRayleighScattering.light.color.xyz * kRayleighScattering.light.intensity * rayleighScatter;

	output.color.xyz *= scatterColor;*/

	return output;
}
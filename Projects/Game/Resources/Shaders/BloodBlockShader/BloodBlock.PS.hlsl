#include "BloodBlock.hlsli"
#include "../PerlinNoise.hlsli"

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;
	
	float32_t4 textureColor = textures[input.textureID].Sample(smp, input.uv);
	output.color = textureColor;

	// ディレクションライト拡散反射光
	float32_t t = dot(input.normal, kLight.ligDirection);

	t *= -1.0f;
	t = (t + abs(t)) * 0.5f;

	float32_t3 diffDirection = kLight.ligColor * t;

	float32_t3 lig = diffDirection;
	lig.x += 0.2f;
	lig.y += 0.2f;
	lig.z += 0.2f;

	output.color.xyz *= lig;

	if(kBloodState[input.instanceID].isBlood == 1) {
		float32_t3 bloodColor;

		float32_t noise = CreateNoise(
			input.uv, 
			kBloodState[input.instanceID].bloodVector.xy,
			0.5f
			);
		bloodColor.r = 0.1f;
		bloodColor.g = 0.8f;
		bloodColor.b = 0.8f;

		noise *= 100.0f;

		if(noise < 0.05f){
			noise = 0.0f;
		}

		output.color.xyz -= bloodColor * noise;
	}

	output.color *= kColor[input.instanceID].color;
	return output;
}
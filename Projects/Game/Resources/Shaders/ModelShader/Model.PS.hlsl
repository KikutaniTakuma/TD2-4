#include "Model.hlsli"

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;
	
	float32_t4 textureColor = textures[input.textureID].Sample(smp, input.uv);

	output.color = textureColor * kColor[input.instanceID].color;

	if(kIsLighting[input.instanceID].isLighting == 1) {
	// ディレクションライト拡散反射光
		float t = dot(input.normal, kLight.ligDirection);

		t *= -1.0f;
		t = (t + abs(t)) * 0.5f;

		float3 diffDirection = kLight.ligColor * t;

		float3 lig = diffDirection;
		lig.x += 0.2f;
		lig.y += 0.2f;
		lig.z += 0.2f;

		output.color.xyz *= lig;
	}
	return output;
}
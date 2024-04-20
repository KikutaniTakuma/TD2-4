#include "Texture2D.hlsli"

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;

    input.position = mul(input.position, kWvpMat[instanceID].worldMat);
	output.worldPosition = input.position;
	output.position = mul(input.position, kWvpMat[instanceID].cameraMat);
	input.normal = normalize(input.normal);
	output.normal = mul(input.normal, (float32_t3x3)kWvpMat[instanceID].worldMat);

	float32_t4 uv = float32_t4(input.uv, 0.0f, 1.0f);
	output.uv = mul(uv, kTexture2DData[instanceID].uvTransform).xy;
	output.instanceID = instanceID;
	output.textureID = input.textureID;

    return output;
}
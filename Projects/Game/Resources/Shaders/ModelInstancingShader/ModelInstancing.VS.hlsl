#include "ModelInstancing.hlsli"

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t3 normal : NORMAL0;
	float32_t2 uv : TEXCOORD;
};

struct WorldViewProjection{
	float32_t4x4 worldMat;
	float32_t4x4 viewProjectionMat;
};
StructuredBuffer<WorldViewProjection> wvpMat: register(t1);


VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
	VertexShaderOutput output;

	input.position = mul(input.position, wvpMat[instanceId].worldMat);
	output.worldPosition = input.position;
	output.position = mul(input.position, wvpMat[instanceId].viewProjectionMat);
	input.normal = normalize(input.normal);
	output.normal = mul(input.normal, (float32_t3x3)wvpMat[instanceId].worldMat);
	output.uv = input.uv;

	return output;
}
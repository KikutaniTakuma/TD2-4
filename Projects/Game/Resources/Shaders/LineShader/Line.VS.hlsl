#include "Line.hlsli"

struct VertxData {
	float4x4 wvp;
	float4 color;
};

StructuredBuffer<VertxData> vertxData: register(t0);

VertexOutput main(float4 pos : POSITION0, uint32_t instanceId : SV_InstanceID) {
    VertexOutput output;
    output.pos = mul(pos,vertxData[instanceId].wvp);
    output.color = vertxData[instanceId].color;

    return output;
}
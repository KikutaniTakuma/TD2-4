#include "../Lamb.hlsli"

struct IsLighting {
	uint32_t isLighting;
};

StructuredBuffer<IsLighting> kIsLighting : register(t2);
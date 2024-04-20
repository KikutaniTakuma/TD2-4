#include "../Lamb.hlsli"

struct BloodState {
	float32_t3 bloodVector
	uint32_t isBlood;
};

StructuredBuffer<BloodState> kBloodState : register(t2);
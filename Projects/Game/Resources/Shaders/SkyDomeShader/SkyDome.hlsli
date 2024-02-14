cbuffer WorldViewProjection : register(b0){
	float32_t4x4 worldMat;
	float32_t4x4 viewProjectionMat;
}

struct IncidentLight {
	float32_t4 color;
	float32_t3 direction;
	float32_t3 pos;
	float32_t intensity;
};

struct AirStatus {
	float32_t refractiveIndex;
	float32_t moleculesNum;
	float32_t scaleFilter;
	float32_t wavelength;
};

struct RayleighScatteringStatus {
	IncidentLight light;
	AirStatus air;
	float32_t3 viewDirection;
	float32_t3 cameraPos;
};

ConstantBuffer<RayleighScatteringStatus> kRayleighScattering : register(b1);

struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
};
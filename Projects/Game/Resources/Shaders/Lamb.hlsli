struct WVPMatrix {
	float32_t4x4 worldMat;
	float32_t4x4 cameraMat;
};

struct DirectionLight {
	float32_t3 ligDirection;
	float32_t3 ligColor;
};

struct Color {
	float32_t4 color;
};

ConstantBuffer<DirectionLight> kLight : register(b0);
StructuredBuffer<WVPMatrix> kWvpMat : register(t0);
StructuredBuffer<Color> kColor : register(t1);
Texture2D<float32_t4> textures[] : register(t3);
SamplerState smp : register(s0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t3 normal : NORMAL0;
	float32_t2 uv : TEXCOORD;
	uint32_t textureID : BLENDINDICES;
};

struct VertexShaderOutput{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
	uint32_t textureID : BLENDINDICES0;
	uint32_t instanceID : BLENDINDICES1;
};

struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};


/// Please define the following in each Shader ///

/// Please define the structure used in each Shader
/// example : 
///	struct IsLighting {
///		uint32_t isLighting;
///	};

/// After defining the structure used in each shader, 
/// define StructuredBuffer and specify register(t2).
/// example : 
/// StructuredBuffer<IsLighting> kIsLighting : register(t2);
#include "Texture2D.hlsli"

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;

    uint32_t textureID = kTexture2DData[input.instanceID].textureID;
	
	float32_t4 textureColor = textures[textureID].Sample(smp, input.uv);

	output.color = textureColor * kColor[input.instanceID].color;
    
	return output;
}
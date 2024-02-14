#include "Texture2D.hlsli"

cbuffer Matrix : register(b0){
    float4x4 mat;
} 

Output main(float4 pos : POSITION, float2 uv : TEXCOORD){
    Output output;

    output.svPos = mul(pos, mat);
    output.uv = uv;

    return output;
}
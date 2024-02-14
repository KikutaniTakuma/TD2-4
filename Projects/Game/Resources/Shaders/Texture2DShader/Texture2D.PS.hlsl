#include "Texture2D.hlsli"

cbuffer ColorPibot :register(b2){
    float pibot;
}

cbuffer ColorPibot :register(b3){
    int colorType;
}

float4 main(Output input) : SV_TARGET{
    float w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0.0f,0.0f,0.0f,0.0f);

    // 最上段
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 2.0f * dy)) * 1.0f;
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 2.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 2.0f * dy)) * 6.0f;
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 2.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 2.0f * dy)) * 1.0f;

    // 一つ上
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 1.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 1.0f * dy)) * 16.0f;
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 1.0f * dy)) * 24.0f;
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 1.0f * dy)) * 16.0f;
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 1.0f * dy)) * 4.0f;

    // 真ん中
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 0.0f * dy)) * 6.0f;
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 0.0f * dy)) * 24.0f;
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 0.0f * dy)) * 36.0f;
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 0.0f * dy)) * 24.0f;
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 0.0f * dy)) * 6.0f;

     // 一つ下
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, -1.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, -1.0f * dy)) * 16.0f;
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, -1.0f * dy)) * 24.0f;
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, -1.0f * dy)) * 16.0f;
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, -1.0f * dy)) * 4.0f;
    
     // 最下段
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, -2.0f * dy)) * 1.0f;
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, -2.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, -2.0f * dy)) * 6.0f;
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, -2.0f * dy)) * 4.0f;
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, -2.0f * dy)) * 1.0f;

    return (ret / 256.0f) * color;
}
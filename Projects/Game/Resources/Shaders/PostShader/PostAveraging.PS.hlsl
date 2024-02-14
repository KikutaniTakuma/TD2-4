#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0.0f,0.0f,0.0f,0.0f);

    // 最上段
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 2.0f * dy));

    // 一つ上
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 1.0f * dy));

    // 真ん中
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 0.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, 0.0f * dy));

     // 一つ下
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, -1.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, -1.0f * dy));
    
     // 最下段
    ret += tex.Sample(smp, input.uv + float2(-2.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, -2.0f * dy));
    ret += tex.Sample(smp, input.uv + float2( 2.0f * dx, -2.0f * dy));

    return ret / 25.0f;
}
#include "Post.hlsli"

float4 main(Output input) : SV_TARGET{
    float w, h, levels;
    tex.GetDimensions(0, w,h,levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0.0f,0.0f,0.0f,0.0f);

    // 上
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 1.0f * dy));
    // 左
    ret += tex.Sample(smp, input.uv + float2(-1.0f * dx, 0.0f * dy));
    // 自分自身
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, 0.0f * dy)) * -4.0f;
    // 右
    ret += tex.Sample(smp, input.uv + float2( 1.0f * dx, 0.0f * dy));
    // 下
    ret += tex.Sample(smp, input.uv + float2( 0.0f * dx, -1.0f * dy));

    // グレースケール化
    float Y = dot(ret.xyz, float3(0.299f, 0.587f, 0.114f));

    Y = pow(1.0f-Y,10.0f);

    clip(0.5f - Y);

    ret.a = tex.Sample(smp, input.uv).a;

    return float4(Y,Y,Y,ret.a);
}
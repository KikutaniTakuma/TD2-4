#include "Post.hlsli"
cbuffer Wipe : register(b0){
    float2 center;
    float wipeSize;
}

float4 main(Output input) : SV_TARGET0 {
    float4 texColor = tex.Sample(smp, input.uv);

    float Y = (texColor.x + texColor.y + texColor.z) / 3.0f;
    float4 monochromoe = float4(Y,Y,Y,texColor.w);

    float2 posFromSenter = input.svPos.xy - center;

    float isWipe = clamp(wipeSize - length(posFromSenter), 0.0f,1.0f);

    float4 output = lerp(monochromoe, texColor, isWipe);

    return output;
}
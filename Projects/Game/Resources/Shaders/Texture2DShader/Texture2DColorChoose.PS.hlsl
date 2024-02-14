#include "Texture2D.hlsli"

cbuffer ColorPibot :register(b2){
    float pibot;
}

cbuffer ColorPibot :register(b3){
    int colorType;
}

float4 main(Output input) : SV_TARGET{
    float4 texColor = tex.Sample(smp, input.uv);

    float Y = (texColor.x + texColor.y + texColor.z) / 3.0f;

    float4 monochro = {Y,Y,Y,texColor.w};

    //texColor.yz *= 0.0f;

    float s;

    if(colorType == 0){
        texColor.x -= pibot;
        texColor.x = (texColor.x + abs(texColor.x)) * 0.5f;
        texColor.x = ceil(texColor.x);
        s = texColor.x;
    }
    else  if(colorType == 1){
        texColor.y -= pibot;
        texColor.y = (texColor.y + abs(texColor.y)) * 0.5f;
        texColor.y = ceil(texColor.y);
        s = texColor.y;
    }
    else  if(colorType == 2){
        texColor.z -= pibot;
        texColor.z = (texColor.z + abs(texColor.z)) * 0.5f;
        texColor.z = ceil(texColor.z);
        s = texColor.z;
    }
    else {
       s = pibot;
       float4 finalColor = lerp(texColor, monochro, s);
       return finalColor;
    }
   

    float4 finalColor = lerp(monochro, texColor, s) * color;

    return finalColor;
}
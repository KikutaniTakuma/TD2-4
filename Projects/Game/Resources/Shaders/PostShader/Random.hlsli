float Random(float2 uv){
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float2 RandomVector(float2 fact){
    float2 angle = float2(
        dot(fact, float2(127.1f, 311.7f)),
        dot(fact, float2(269.5f, 183.3f))
    );

    return frac(sin(angle) * 43758.5453123f) * 2.0f - 1.0f;
}
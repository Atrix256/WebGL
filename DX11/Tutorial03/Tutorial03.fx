//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 SampleTime(float2 time, bool linearSampling) {
    return txDiffuse.Sample(samLinear, time);
    /*
    // Bilinear sampling:
    // Hardware based bilinear sampling
    if (linearSampling)
        return texture(uSampler, (time + 0.5) / 2.0);

    // Nearest sampling:
    // Software bilinear sampling (higher quality)
    vec2 frac = fract(time);

    vec2 floorTime = floor(time) + 0.5;

    vec4 A = texture(uSampler, (floorTime + vec2(0.0, 0.0)) / 2.0);
    vec4 B = texture(uSampler, (floorTime + vec2(1.0, 0.0)) / 2.0);
    vec4 C = texture(uSampler, (floorTime + vec2(0.0, 1.0)) / 2.0);
    vec4 D = texture(uSampler, (floorTime + vec2(1.0, 1.0)) / 2.0);

    return mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);
    */
}

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = (float4)0.0;
    float4 curveValues = (float4)0.0;

    if (input.Tex.x < 0.995)
    {
        float time = input.Tex.x / 0.995;
        float2 samplePos = (float2)((time + 0.5) / 2.0);
        curveValues = SampleTime(samplePos, true);
    }
    else if (input.Tex.x > 1.005)
    {
        float time = frac(input.Tex.x - 0.005) / 0.995;
        float2 samplePos = (float2)((time + 0.5) / 2.0);
        curveValues = SampleTime(samplePos, true);
    }
    else
    {
        return (float4)1.0;
    }

    if (input.Tex.y < curveValues.x)
        color.y = 1.0;

    return color;
}
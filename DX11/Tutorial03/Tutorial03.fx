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

    // Bilinear sampling:
    // Hardware based bilinear sampling
    if (linearSampling)
        return txDiffuse.Sample(samLinear, (time + 0.5) / 2.0);

    // Nearest sampling:
    // Software bilinear sampling (higher quality)
    float2 fract = frac(time);

    float2 floorTime = floor(time) + 0.5;

    float4 A = txDiffuse.Sample(samLinear, (floorTime + float2(0.0, 0.0)) / 2.0);
    float4 B = txDiffuse.Sample(samLinear, (floorTime + float2(1.0, 0.0)) / 2.0);
    float4 C = txDiffuse.Sample(samLinear, (floorTime + float2(0.0, 1.0)) / 2.0);
    float4 D = txDiffuse.Sample(samLinear, (floorTime + float2(1.0, 1.0)) / 2.0);

    return lerp(lerp(A, B, fract.x), lerp(C, D, fract.x), fract.y);
}

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = (float4)0.0;
    float4 curveValues = (float4)0.0;

    if (input.Tex.x < 0.995)
    {
        float time = input.Tex.x / 0.995;
        curveValues = SampleTime((float2)time, true);
    }
    else if (input.Tex.x > 1.005)
    {
        float time = frac(input.Tex.x - 0.005) / 0.995;
        curveValues = SampleTime((float2)time, false);
    }
    else
    {
        return (float4)1.0;
    }

    if (input.Tex.y < curveValues.x)
        color.y = 1.0;

    return color;
}
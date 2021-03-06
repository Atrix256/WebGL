#include "Shaders.h"

//=============================================================================================================
//                                            Cubic2d
//=============================================================================================================

void CShaderCubic2d::Init()
{
    SetAttributeData_aTextureCoord({
        0.0, 0.0,
        2.0, 0.0,
        2.0, 1.0,
        0.0, 0.0,
        2.0, 1.0,
        0.0, 1.0,
    });
    SetAttributeData_aVertexPosition({
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
        -1.0, -1.0,
         1.0,  1.0,
        -1.0,  1.0,
    });

    const float R[] = { 0.5f , 0.0f, 0.75f, 0.2f };
    const float G[] = { 0.0f , 1.0f, 0.25f, 0.8f };
    const float B[] = { 0.2f , 0.0f, 1.00f, 0.6f };
    const float A[] = { 0.75f, 0.0f, 0.29f, 0.4f };

    SetTextureData_uSampler(2, 4, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],
        R[2], G[2], B[2], A[2],     R[3], G[3], B[3], A[3],
        0.0f, 0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 0.0f,
    });
}

const char *CShaderCubic2d::GetVertexShader()
{
    return
    SHADER_SOURCE(
    in vec2 aVertexPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main(void) {
        gl_Position = vec4(aVertexPosition, 1.0, 1.0);
        vTextureCoord = aTextureCoord;
    }
    );
}

const char *CShaderCubic2d::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(vec2 time, bool linearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (linearSampling)
            return texture(uSampler, (time + 0.5) / vec2(2.0, 4.0));

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        vec2 frac = fract(time);

        vec2 floorTime = floor(time) + 0.5;

        vec4 A = texture(uSampler, (floorTime + vec2(0.0, 0.0)) / vec2(2.0, 4.0));
        vec4 B = texture(uSampler, (floorTime + vec2(1.0, 0.0)) / vec2(2.0, 4.0));
        vec4 C = texture(uSampler, (floorTime + vec2(0.0, 1.0)) / vec2(2.0, 4.0));
        vec4 D = texture(uSampler, (floorTime + vec2(1.0, 1.0)) / vec2(2.0, 4.0));

        return mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);
    }

    void main(void) {
        vec4 colorValue = vec4(0.0);

        if (vTextureCoord.x < 0.995)
        {
            float time = vTextureCoord.x / 0.995;
            colorValue = SampleTime(vec2(time), true);
            colorValue = mix(colorValue, SampleTime(vec2(time, time + 1.0), true), time);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            colorValue = SampleTime(vec2(time), false);
            colorValue = mix(colorValue, SampleTime(vec2(time, time + 1.0), false), time);
        }
        else
        {
            outColor = vec4(1.0);
            return;
        }

        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}


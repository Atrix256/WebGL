#include "Shaders.h"

//=============================================================================================================
//                                            Quadratic1d
//=============================================================================================================

void CShaderQuadratic1d::Init()
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

    const float R[] = { 0.5f , 0.0f, 0.75f };
    const float G[] = { 0.0f , 1.0f, 0.25f };
    const float B[] = { 0.2f , 0.0f, 1.00f };
    const float A[] = { 0.75f, 0.0f, 0.29f };

    SetTextureData_uSampler(4, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],     0.0f, 0.0f, 0.0f, 0.0f
    });
}

const char *CShaderQuadratic1d::GetVertexShader()
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

const char *CShaderQuadratic1d::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler1D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float time, bool linearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (linearSampling)
            return texture(uSampler, (time + 0.5) / 4.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        float frac = fract(time);

        float floorTime = floor(time) + 0.5;

        vec4 A = texture(uSampler, (floorTime + 0.0) / 4.0);
        vec4 B = texture(uSampler, (floorTime + 1.0) / 4.0);

        return mix(A, B, frac);
    }

    void main(void) {
        vec4 colorValue = vec4(0.0);

        if (vTextureCoord.x < 0.995)
        {
            float time = vTextureCoord.x / 0.995;
            colorValue = SampleTime(time, true);
            colorValue = mix(colorValue, SampleTime(time + 1.0, true), time);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            colorValue = SampleTime(time, false);
            colorValue = mix(colorValue, SampleTime(time + 1.0, false), time);
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


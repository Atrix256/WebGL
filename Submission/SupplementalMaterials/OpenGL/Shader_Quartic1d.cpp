#include "Shaders.h"

//=============================================================================================================
//                                            Quartic1d
//=============================================================================================================

void CShaderQuartic1d::Init()
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

    const float R[] = { 0.5f , 0.0f, 0.75f, 0.2f, 0.7f };
    const float G[] = { 0.0f , 1.0f, 0.25f, 0.8f, 0.6f };
    const float B[] = { 0.2f , 0.0f, 1.00f, 0.6f, 0.3f };
    const float A[] = { 0.75f, 0.0f, 0.29f, 0.4f, 1.0f };

    SetTextureData_uSampler(8, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],     R[3], G[3], B[3], A[3],
        R[4], G[4], B[4], A[4],     0.0f, 0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 0.0f,
    });
}

const char *CShaderQuartic1d::GetVertexShader()
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

const char *CShaderQuartic1d::GetFragmentShader()
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
            return texture(uSampler, (time + 0.5) / 8.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        float frac = fract(time);

        float floorTime = floor(time) + 0.5;

        vec4 A = texture(uSampler, (floorTime + 0.0) / 8.0);
        vec4 B = texture(uSampler, (floorTime + 1.0) / 8.0);

        return mix(A, B, frac);
    }

    vec4 CombineCubic(float t, vec4 A, vec4 B, vec4 C, vec4 D) {
        float t2 = t * t;
        float t3 = t * t * t;
        float s = 1.0 - t;
        float s2 = s * s;
        float s3 = s * s * s;

        return
            A * 1.0 * s3 +
            B * 3.0 * s2 * t +
            C * 3.0 * s * t2 +
            D * 1.0 * t3;
    }

    void main(void) {
        vec4 colorValue = vec4(0.0);

        if (vTextureCoord.x < 0.995)
        {
            float time = vTextureCoord.x / 0.995;

            vec4 colorValueA = SampleTime(time + 0.0, true);
            vec4 colorValueB = SampleTime(time + 1.0, true);
            vec4 colorValueC = SampleTime(time + 2.0, true);
            vec4 colorValueD = SampleTime(time + 3.0, true);

            colorValue = CombineCubic(time, colorValueA, colorValueB, colorValueC, colorValueD);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;

            vec4 colorValueA = SampleTime(time + 0.0, false);
            vec4 colorValueB = SampleTime(time + 1.0, false);
            vec4 colorValueC = SampleTime(time + 2.0, false);
            vec4 colorValueD = SampleTime(time + 3.0, false);

            colorValue = CombineCubic(time, colorValueA, colorValueB, colorValueC, colorValueD);
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


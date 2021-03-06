#include "Shaders.h"

//=============================================================================================================
//                                            Quadratic2d
//=============================================================================================================

void CShaderQuadratic2d::Init()
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

    SetTextureData_uSampler(2, 2, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2]
    });
}

const char *CShaderQuadratic2d::GetVertexShader()
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

const char *CShaderQuadratic2d::GetFragmentShader()
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
    }

    void main(void) {
        vec4 colorValue = vec4(0.0);

        if (vTextureCoord.x < 0.995)
        {
            float time = vTextureCoord.x / 0.995;
            colorValue = SampleTime(vec2(time), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            colorValue = SampleTime(vec2(time), false);
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


#include "Shaders.h"

//=============================================================================================================
//                                            BilinearTest
//=============================================================================================================

void CShaderBilinearTest::Init()
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

    float A = 0.5f;
    float B = 0.2f;
    float C = 0.8f;

    SetTextureData_uSampler(2, 2, {
        A, 0.0f, 0.0f, 0.0f,     B, 0.0f, 0.0f, 0.0f,
        B, 0.0f, 0.0f, 0.0f,     C, 0.0f, 0.0f, 0.0f
    });
}

const char *CShaderBilinearTest::GetVertexShader()
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

const char *CShaderBilinearTest::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    bool PixelInControlPoint(vec2 pixel) {

        vec4 A = texture(uSampler, vec2(0.5, 0.5) / 2.0);
        vec4 B = texture(uSampler, vec2(1.5, 0.5) / 2.0);
        vec4 C = texture(uSampler, vec2(1.5, 1.5) / 2.0);

        return
            length(pixel - vec2(0.0, A.x)) < 0.02 ||
            length(pixel - vec2(0.5, B.x)) < 0.02 ||
            length(pixel - vec2(1.0, C.x)) < 0.02;
    }

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
            if (PixelInControlPoint(vec2(time, vTextureCoord.y)))
            {
                outColor = vec4(1.0);
                return;
            }
            colorValue = SampleTime(vec2(time), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            if (PixelInControlPoint(vec2(time, vTextureCoord.y)))
            {
                outColor = vec4(1.0);
                return;
            }
            colorValue = SampleTime(vec2(time), false);
        }
        else
        {
            outColor = vec4(1.0);
            return;
        }

        float value = step(vTextureCoord.y, colorValue.x);
        outColor = vec4(0.0, value, 0.0, 1.0);
    }
    );
}


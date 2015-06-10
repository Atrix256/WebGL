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

    SetTextureData_uSampler(2, 2, {
        128, 0, 0, 255,      50, 0, 0, 255,
         50, 0, 0, 255,     200, 0, 0, 255
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

    vec4 SamplePixel(vec2 pixel, bool bilinearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (bilinearSampling)
            return texture2D(uSampler, (pixel + vec2(0.5)) / 2.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        float fracu = fract(pixel.x);
        float fracv = fract(pixel.y);

        vec2 floorPixel = floor(pixel) + vec2(0.5, 0.5);

        vec4 A = texture2D(uSampler, floorPixel / 2.0);
        vec4 B = texture2D(uSampler, (floorPixel + vec2(1.0, 0.0)) / 2.0);
        vec4 C = texture2D(uSampler, (floorPixel + vec2(0.0, 1.0)) / 2.0);
        vec4 D = texture2D(uSampler, (floorPixel + vec2(1.0, 1.0)) / 2.0);

        return mix(mix(A, B, fracu), mix(C, D, fracu), fracv);
    }

    void main(void) {
        vec4 colorValue;

        if (vTextureCoord.x < 0.995)
        {
            float pixel = vTextureCoord.x / 0.995;
            colorValue = SamplePixel(vec2(pixel), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float pixel = fract(vTextureCoord.x - 0.005) / 0.995;
            colorValue = SamplePixel(vec2(pixel), false);
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


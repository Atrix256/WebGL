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

    float A = 0.5f;
    float B = 0.2f;
    float C = 0.8f;

    SetTextureData_uSampler(2, 2, {
        A, 0.0f, 0.0f, 0.0f,     B, 0.0f, 0.0f, 0.0f,
        B, 0.0f, 0.0f, 0.0f,     C, 0.0f, 0.0f, 0.0f
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

    vec4 SamplePixel(vec2 pixel, bool linearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (linearSampling)
            return texture(uSampler, (pixel + 0.5) / 2.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        vec2 frac = fract(pixel);

        vec2 floorPixel = floor(pixel) + 0.5;

        vec4 A = texture(uSampler, (floorPixel + vec2(0.0, 0.0)) / 2.0);
        vec4 B = texture(uSampler, (floorPixel + vec2(1.0, 0.0)) / 2.0);
        vec4 C = texture(uSampler, (floorPixel + vec2(0.0, 1.0)) / 2.0);
        vec4 D = texture(uSampler, (floorPixel + vec2(1.0, 1.0)) / 2.0);

        return mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);
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
        outColor = vec4(0.0, 0.0, value, 1.0);
    }
    );
}


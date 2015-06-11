#include "Shaders.h"

//=============================================================================================================
//                                            TrilinearTest
//=============================================================================================================

void CShaderTrilinearTest::Init()
{
    SetAttributeData_aTextureCoord({
        0.0, 0.0, 0.0,
        2.0, 0.0, 0.0,
        2.0, 1.0, 0.0,
        0.0, 0.0, 0.0,
        2.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
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
    float D = 0.1f;

    SetTextureData_uSampler(2, 2, 2, {
        A, 0.0f, 0.0f, 0.0f,     B, 0.0f, 0.0f, 0.0f,
        B, 0.0f, 0.0f, 0.0f,     C, 0.0f, 0.0f, 0.0f,

        B, 0.0f, 0.0f, 0.0f,     C, 0.0f, 0.0f, 0.0f,
        C, 0.0f, 0.0f, 0.0f,     D, 0.0f, 0.0f, 0.0f
    });
}

const char *CShaderTrilinearTest::GetVertexShader()
{
    return
    SHADER_SOURCE(
    in vec2 aVertexPosition;
    in vec3 aTextureCoord;

    out vec3 vTextureCoord;

    void main(void) {
        gl_Position = vec4(aVertexPosition, 1.0, 1.0);
        vTextureCoord = aTextureCoord;
    }
    );
}

const char *CShaderTrilinearTest::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec3 vTextureCoord;

    out vec4 outColor;

    vec4 SamplePixel(vec3 pixel, bool linearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (linearSampling)
            return texture(uSampler, (pixel + 0.5) / 2.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        vec3 frac = fract(pixel);

        vec3 floorPixel = floor(pixel) + 0.5;

        vec4 A = texture(uSampler, (floorPixel + vec3(0.0, 0.0, 0.0)) / 2.0);
        vec4 B = texture(uSampler, (floorPixel + vec3(1.0, 0.0, 0.0)) / 2.0);
        vec4 C = texture(uSampler, (floorPixel + vec3(0.0, 1.0, 0.0)) / 2.0);
        vec4 D = texture(uSampler, (floorPixel + vec3(1.0, 1.0, 0.0)) / 2.0);
        vec4 front = mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);

        vec4 E = texture(uSampler, (floorPixel + vec3(0.0, 0.0, 1.0)) / 2.0);
        vec4 F = texture(uSampler, (floorPixel + vec3(1.0, 0.0, 1.0)) / 2.0);
        vec4 G = texture(uSampler, (floorPixel + vec3(0.0, 1.0, 1.0)) / 2.0);
        vec4 H = texture(uSampler, (floorPixel + vec3(1.0, 1.0, 1.0)) / 2.0);
        vec4 back = mix(mix(E, F, frac.x), mix(G, H, frac.x), frac.y);

        return mix(front, back, frac.z);
    }

    void main(void) {
        vec4 colorValue;

        if (vTextureCoord.x < 0.995)
        {
            float pixel = vTextureCoord.x / 0.995;
            colorValue = SamplePixel(vec3(pixel), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float pixel = fract(vTextureCoord.x - 0.005) / 0.995;
            colorValue = SamplePixel(vec3(pixel), false);
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


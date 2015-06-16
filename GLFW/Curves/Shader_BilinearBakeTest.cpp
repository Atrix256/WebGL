#include "Shaders.h"

//=============================================================================================================
//                                            BilinearBakeTest
//=============================================================================================================

void CShaderBilinearBakeTest::Init()
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

    // 4x1 baked out texture for curve to show accuracy using same number of pixels as 2x2 texture
    const int c_numBakeSamples = 4;
    std::vector<float> curvePoints;
    for (int i = 0; i < c_numBakeSamples; ++i) {
        float t = float(i) / float(c_numBakeSamples - 1);
        float t2 = t*t;
        float s = 1.0f - t;
        float s2 = s*s;
        float curveValue = A*s2 + B*2.0f*s*t + C*t2;
        curvePoints.push_back(curveValue);
        curvePoints.push_back(0.0f);
        curvePoints.push_back(0.0f);
        curvePoints.push_back(0.0f);
    }
    SetTextureData_uBakedSampler(c_numBakeSamples, 1, curvePoints);
    SetUniformData_uBakeSamples((float)c_numBakeSamples);
}

const char *CShaderBilinearBakeTest::GetVertexShader()
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

const char *CShaderBilinearBakeTest::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    uniform sampler2D uBakedSampler;
    uniform float     uBakeSamples;
    
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

    float SampleTimeBaked(float time) {
        return texture(uBakedSampler, vec2((time*(uBakeSamples - 1) / uBakeSamples) + 1.0 / (uBakeSamples * 2), 0.5)).x;
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
            colorValue.y = SampleTimeBaked(time);
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
            colorValue.y = SampleTimeBaked(time);
        }
        else
        {
            outColor = vec4(1.0);
            return;
        }

        float value = step(vTextureCoord.y, colorValue.x);
        float bakedValue = step(vTextureCoord.y, colorValue.y);
        outColor = vec4(bakedValue, value, 0.0, 1.0);
    }
    );
}


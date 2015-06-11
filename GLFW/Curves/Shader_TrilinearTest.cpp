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

    bool PixelInControlPoint(vec2 pixel) {

        vec4 A = texture(uSampler, vec3(0.25, 0.25, 0.25));
        vec4 B = texture(uSampler, vec3(0.75, 0.25, 0.25));
        vec4 C = texture(uSampler, vec3(0.75, 0.75, 0.25));
        vec4 D = texture(uSampler, vec3(0.75, 0.75, 0.75));

        return
            length(pixel - vec2(0.0, A.x)) < 0.02 ||
            length(pixel - vec2(0.3, B.x)) < 0.02 ||
            length(pixel - vec2(0.6, C.x)) < 0.02 ||
            length(pixel - vec2(1.0, D.x)) < 0.02;
    }

    vec4 SampleTime(vec3 time, bool linearSampling) {
        // Trilinear sampling:
        // Hardware based trilinear sampling
        if (linearSampling)
            return texture(uSampler, (time + 0.5) / 2.0);

        // Nearest sampling:
        // Software trilinear sampling (higher quality)
        vec3 frac = fract(time);

        vec3 floorTime = floor(time) + 0.5;

        vec4 A = texture(uSampler, (floorTime + vec3(0.0, 0.0, 0.0)) / 2.0);
        vec4 B = texture(uSampler, (floorTime + vec3(1.0, 0.0, 0.0)) / 2.0);
        vec4 C = texture(uSampler, (floorTime + vec3(0.0, 1.0, 0.0)) / 2.0);
        vec4 D = texture(uSampler, (floorTime + vec3(1.0, 1.0, 0.0)) / 2.0);
        vec4 front = mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);

        vec4 E = texture(uSampler, (floorTime + vec3(0.0, 0.0, 1.0)) / 2.0);
        vec4 F = texture(uSampler, (floorTime + vec3(1.0, 0.0, 1.0)) / 2.0);
        vec4 G = texture(uSampler, (floorTime + vec3(0.0, 1.0, 1.0)) / 2.0);
        vec4 H = texture(uSampler, (floorTime + vec3(1.0, 1.0, 1.0)) / 2.0);
        vec4 back = mix(mix(E, F, frac.x), mix(G, H, frac.x), frac.y);

        return mix(front, back, frac.z);
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
            colorValue = SampleTime(vec3(time), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            if (PixelInControlPoint(vec2(time, vTextureCoord.y)))
            {
                outColor = vec4(1.0);
                return;
            }
            colorValue = SampleTime(vec3(time), false);
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


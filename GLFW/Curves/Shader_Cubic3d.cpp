#include "Shaders.h"

//=============================================================================================================
//                                            Cubic3d
//=============================================================================================================

void CShaderCubic3d::Init()
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

    const float R[] = { 0.5f , 0.0f, 0.75f, 0.2f };
    const float G[] = { 0.0f , 1.0f, 0.25f, 0.8f };
    const float B[] = { 0.2f , 0.0f, 1.00f, 0.6f };
    const float A[] = { 0.75f, 0.0f, 0.29f, 0.4f };

    SetTextureData_uSampler(2, 2, 2, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],

        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],
        R[2], G[2], B[2], A[2],     R[3], G[3], B[3], A[3],
    });
}

const char *CShaderCubic3d::GetVertexShader()
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

const char *CShaderCubic3d::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec3 vTextureCoord;

    out vec4 outColor;

    bool PixelInControlPoint(vec2 pixel) {

        vec4 A = texture(uSampler, vec3(0.5, 0.5, 0.5) / vec3(2.0, 2.0, 2.0));
        vec4 B = texture(uSampler, vec3(1.5, 0.5, 0.5) / vec3(2.0, 2.0, 2.0));
        vec4 C = texture(uSampler, vec3(1.5, 1.5, 0.5) / vec3(2.0, 2.0, 2.0));
        vec4 D = texture(uSampler, vec3(1.5, 1.5, 1.5) / vec3(2.0, 2.0, 2.0));

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
                outColor = vec4(0.7, 0.7, 0.7, 1.0);
                return;
            }
            colorValue = SampleTime(vec3(time), true);
        }
        else if (vTextureCoord.x > 1.005)
        {
            float time = fract(vTextureCoord.x - 0.005) / 0.995;
            if (PixelInControlPoint(vec2(time, vTextureCoord.y)))
            {
                outColor = vec4(0.7, 0.7, 0.7, 1.0);
                return;
            }
            colorValue = SampleTime(vec3(time), false);
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


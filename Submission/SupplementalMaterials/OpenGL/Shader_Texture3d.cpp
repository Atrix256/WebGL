#include "Shaders.h"

//=============================================================================================================
//                                            Texture3d
//=============================================================================================================

void CShaderTexture3d::Init()
{
    SetAttributeData_aTextureCoord({
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.2,
        1.0, 1.0, 0.4,
        0.0, 0.0, 0.6,
        1.0, 1.0, 0.8,
        0.0, 1.0, 1.0,
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

    SetTextureData_uSampler(2, 2, 2, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],

        R[1], G[1], B[1], A[1],     R[2], G[2], B[2], A[2],
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
    });
}

const char *CShaderTexture3d::GetVertexShader()
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

const char *CShaderTexture3d::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec3 vTextureCoord;

    out vec4 outColor;

    void main(void) {
        outColor = vec4(texture(uSampler, vTextureCoord).xyz, 1.0);
    }
    );
}


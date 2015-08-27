#include "Shaders.h"

//=============================================================================================================
//                                            Texture1d
//=============================================================================================================

void CShaderTexture1d::Init()
{
    SetAttributeData_aTextureCoord({
        0.0,
        1.0,
        1.0,
        0.0,
        1.0,
        0.0,
    });
    SetAttributeData_aVertexPosition({
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
        -1.0, -1.0,
         1.0,  1.0,
        -1.0,  1.0,
    });

    const float R[] = { 0.5f , 0.0f};
    const float G[] = { 0.0f , 1.0f};
    const float B[] = { 0.2f , 0.0f};
    const float A[] = { 0.75f, 0.0f};

    SetTextureData_uSampler(2, {
        R[0], G[0], B[0], A[0],     R[1], G[1], B[1], A[1],
    });
}

const char *CShaderTexture1d::GetVertexShader()
{
    return
    SHADER_SOURCE(
    in vec2 aVertexPosition;
    in float aTextureCoord;

    out float vTextureCoord;

    void main(void) {
        gl_Position = vec4(aVertexPosition, 1.0, 1.0);
        vTextureCoord = aTextureCoord;
    }
    );
}

const char *CShaderTexture1d::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler1D uSampler;
    
    in float vTextureCoord;

    out vec4 outColor;

    void main(void) {
        outColor = vec4(texture(uSampler, vTextureCoord).xyz, 1.0);
    }
    );
}


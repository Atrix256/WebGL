#include "Shaders.h"

//=============================================================================================================
//                                            TrilinearTest3
//=============================================================================================================

void CShaderTrilinearTest3::Init()
{
    SetAttributeData_aTextureCoord({
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0,
        0.0, 0.0, 0.0,
        1.0, 1.0, 0.0,
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

const char *CShaderTrilinearTest3::GetVertexShader()
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

const char *CShaderTrilinearTest3::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec3 vTextureCoord;

    out vec4 outColor;

    void main(void) {
        outColor = vec4(texture(uSampler, vec3((vTextureCoord.x + 0.5) / 2.0)).xyz, 1.0);
    }
    );
}


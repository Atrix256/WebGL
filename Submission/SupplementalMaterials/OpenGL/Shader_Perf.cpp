#include "Shaders.h"

//=============================================================================================================
//                                              Utils
//=============================================================================================================

template <typename T>
void SetTextureAndVertexCoords(T& o)
{
    o.SetAttributeData_aTextureCoord({
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    });
    o.SetAttributeData_aVertexPosition({
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
    });
}

template <typename T>
void Init_Linear1D(T& o)
{
    SetTextureAndVertexCoords(o);
    const unsigned char R[] = { 255,   0 };
    const unsigned char G[] = {   0, 255 };
    const unsigned char B[] = {  50,   0 };
    const unsigned char A[] = { 192,   0 };

    o.SetTextureData_uSampler(2, {
        R[0], G[0], B[0], A[0], R[1], G[1], B[1], A[1]
    });
}

template <typename T>
void Init_Quadratic2D(T& o)
{
    SetTextureAndVertexCoords(o);
    const unsigned char R[] = { 255,   0, 255 };
    const unsigned char G[] = {   0, 255,   0 };
    const unsigned char B[] = {  50,   0, 192 };
    const unsigned char A[] = { 192,  64, 128 };

    o.SetTextureData_uSampler(2, 2, {
        R[0], G[0], B[0], A[0], R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1], R[2], G[2], B[2], A[2]
    });
}

template <typename T>
void Init_Cubic3D(T& o)
{
    SetTextureAndVertexCoords(o);
    const unsigned char R[] = { 255,   0, 255,   0 };
    const unsigned char G[] = {   0, 255,   0, 255 };
    const unsigned char B[] = {  50,   0, 192,  64 };
    const unsigned char A[] = { 192,  64, 128,   0 };

    o.SetTextureData_uSampler(2, 2, 2, {
        R[0], G[0], B[0], A[0], R[1], G[1], B[1], A[1],
        R[1], G[1], B[1], A[1], R[2], G[2], B[2], A[2],

        R[1], G[1], B[1], A[1], R[2], G[2], B[2], A[2],
        R[2], G[2], B[2], A[2], R[3], G[3], B[3], A[3]
    });
}

const char *ShaderSource_Vertex()
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

//=============================================================================================================
//                                        CShaderPerf_Linear_1D_SW
//=============================================================================================================

void CShaderPerf_Linear_1D_SW::Init()
{
    Init_Linear1D(*this);
}

const char *CShaderPerf_Linear_1D_SW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Linear_1D_SW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler1D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float time) {
        vec4 A = texture(uSampler, 0.25);
        vec4 B = texture(uSampler, 0.75);
        return mix(A, B, time);
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}

//=============================================================================================================
//                                        CShaderPerf_Linear_1D_HW
//=============================================================================================================

void CShaderPerf_Linear_1D_HW::Init()
{
    Init_Linear1D(*this);
}

const char *CShaderPerf_Linear_1D_HW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Linear_1D_HW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler1D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float time) {
        return texture(uSampler, (time + 0.5) / 2.0);
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}

//=============================================================================================================
//                                        Perf_Quadratic_2D_SW
//=============================================================================================================

void CShaderPerf_Quadratic_2D_SW::Init()
{
    Init_Quadratic2D(*this);
}

const char *CShaderPerf_Quadratic_2D_SW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Quadratic_2D_SW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float t) {
        vec4 A = texture(uSampler, vec2(0.25, 0.25));
        vec4 B = texture(uSampler, vec2(0.75, 0.25));
        vec4 C = texture(uSampler, vec2(0.75, 0.75));

        float s = 1 - t;
        float s2 = s * s;
        float t2 = t * t;

        return A*s2 + B*2.0*s*t + C*t2;
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}

//=============================================================================================================
//                                        Perf_Quadratic_2D_HW
//=============================================================================================================

void CShaderPerf_Quadratic_2D_HW::Init()
{
    Init_Quadratic2D(*this);
}

const char *CShaderPerf_Quadratic_2D_HW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Quadratic_2D_HW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float t) {
        return texture(uSampler, vec2((t + 0.5) / 2.0));
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}

//=============================================================================================================
//                                        Perf_Cubic_3D_SW
//=============================================================================================================

void CShaderPerf_Cubic_3D_SW::Init()
{
    Init_Cubic3D(*this);
}

const char *CShaderPerf_Cubic_3D_SW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Cubic_3D_SW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float t) {
        vec4 A = texture(uSampler, vec3(0.25, 0.25, 0.25));
        vec4 B = texture(uSampler, vec3(0.75, 0.25, 0.25));
        vec4 C = texture(uSampler, vec3(0.75, 0.75, 0.25));
        vec4 D = texture(uSampler, vec3(0.75, 0.75, 0.75));

        float s = 1 - t;
        float s2 = s * s;
        float s3 = s * s * s;
        float t2 = t * t;
        float t3 = t * t * t;

        return A*s3 + B*3.0*s2*t + C*3.0*s*t2 + D*t3;
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}

//=============================================================================================================
//                                        Perf_Cubic_3D_HW
//=============================================================================================================

void CShaderPerf_Cubic_3D_HW::Init()
{
    Init_Cubic3D(*this);
}

const char *CShaderPerf_Cubic_3D_HW::GetVertexShader()
{
    return ShaderSource_Vertex();
}

const char *CShaderPerf_Cubic_3D_HW::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler3D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(float t) {
        return texture(uSampler, vec3((t + 0.5) / 2.0));
    }

    void main(void) {
        vec4 colorValue = SampleTime(vTextureCoord.x);
        vec4 values = step(vTextureCoord.yyyy, colorValue);
        values.xyz *= 0.5;
        values.xyz += values.w * 0.5;
        outColor = vec4(values.xyz, 1.0);
    }
    );
}
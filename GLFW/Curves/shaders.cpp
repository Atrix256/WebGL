//=============================================================================================================
// Expands ShaderDefs.h into CShader... classes
//=============================================================================================================

#include "GL/glew.h"
#include <windows.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"

//=============================================================================================================
//                                        Shader Program Loading
//=============================================================================================================

template <typename T>
const char *GetVertexShader();

template <typename T>
const char *GetFragmentShader();

template <typename T>
GLuint LoadShader(const T& dummy)
{
    return LoadShader(GetVertexShader<T>(), GetFragmentShader<T>());
}

//=============================================================================================================
//                                            Helper Funcs
//=============================================================================================================

template <typename T>
GLenum GLType();

template <>
GLenum GLType<float>()
{
    return GL_FLOAT;
}

template <>
GLenum GLType<double>()
{
    return GL_DOUBLE;
}

//=============================================================================================================
//                                            Boiler Plate
//=============================================================================================================

// ----- MAKE CONSTRUCTORS ----- 
#define SHADER_BEGIN(NAME) \
    CShader##NAME##::CShader##NAME##() { \
        m_program = LoadShader(*this);
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
        m_attr_##NAME## = glGetAttribLocation(m_program, #NAME); \
        m_attr_##NAME##_buffer = -1; \
        glEnableVertexAttribArray(m_attr_##NAME##);
#define SHADER_UNIFORM_TEXTURE(NAME, TYPE) \
        m_uniform_##NAME## = glGetUniformLocation(m_program, #NAME); \
        m_texture_##NAME## = -1;
#define SHADER_END() \
    } 
#include "ShaderDefs.h"

// ----- MAKE DESTRUCTORS -----
#define SHADER_BEGIN(NAME) \
    CShader##NAME##::~CShader##NAME##() { \
        glDeleteProgram(m_program);
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
#define SHADER_UNIFORM_TEXTURE(NAME, TYPE) \
        glDeleteTextures(1, &m_texture_##NAME##);
#define SHADER_END() \
    } 
#include "ShaderDefs.h"

// ----- MAKE RENDER FUNCTIONS -----
#define SHADER_BEGIN(NAME) \
    void CShader##NAME##::Render() { \
        glUseProgram(m_program);
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
        glBindBuffer(GL_ARRAY_BUFFER, m_attr_##NAME##_buffer); \
        glVertexAttribPointer(m_attr_##NAME##, ELEMENTSIZE, GLType<TYPE>(), false, 0, 0);
#define SHADER_UNIFORM_TEXTURE(NAME, TYPE) \
        glActiveTexture(GL_TEXTURE0); \
        glBindTexture(GL_TEXTURE_2D, m_texture_##NAME##); \
        glUniform1i(m_uniform_##NAME##, 0);
#define SHADER_END() \
        glDrawArrays(GL_TRIANGLES, 0, m_numTriangles); \
    } 
#include "ShaderDefs.h"

// used by shader source code defined below
#define SHADER(...) #__VA_ARGS__

//=============================================================================================================
//                                            BilinearTest
//=============================================================================================================

template<>
const char *GetVertexShader<CShaderBilinearTest>()
{
    return
    SHADER(
    attribute vec2 aVertexPosition;
    attribute vec2 aTextureCoord;

    varying vec2 vTextureCoord;

    void main(void) {
        gl_Position = vec4(aVertexPosition, 1.0, 1.0);
        vTextureCoord = aTextureCoord;
    }
    );
}

template<>
const char *GetFragmentShader<CShaderBilinearTest>()
{
    return
    SHADER(
    uniform sampler2D uSampler;
    varying vec2 vTextureCoord;

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
            gl_FragColor = vec4(1.0);
            return;
        }

        float value = step(vTextureCoord.y, colorValue.x);
        gl_FragColor = vec4(0.0, value, 0.0, 1.0);
    }
    );
}


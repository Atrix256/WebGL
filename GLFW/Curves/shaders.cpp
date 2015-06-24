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
GLuint LoadShader(const T& shader)
{
    return LoadShader(shader.GetVertexShader(), shader.GetFragmentShader());
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
#define SHADER_BEGIN(NAME, WIDTH, HEIGHT) \
    CShader##NAME##::CShader##NAME##() { \
        m_program = LoadShader(*this);
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
        m_attr_##NAME## = glGetAttribLocation(m_program, #NAME); \
        m_attr_##NAME##_buffer = -1; \
        glEnableVertexAttribArray(m_attr_##NAME##);
#define SHADER_UNIFORM(NAME) \
        m_uniform_##NAME## = glGetUniformLocation(m_program, #NAME); \
        m_uniform_value_##NAME## = 0.0f;
#define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE) \
        m_uniform_##NAME## = glGetUniformLocation(m_program, #NAME); \
        m_texture_##NAME## = -1;
#define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE) \
        m_uniform_##NAME## = glGetUniformLocation(m_program, #NAME); \
        m_texture_##NAME## = -1;
#define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE) \
        m_uniform_##NAME## = glGetUniformLocation(m_program, #NAME); \
        m_texture_##NAME## = -1;
#define SHADER_END() \
    } 
#include "ShaderDefs.h"

// ----- MAKE DESTRUCTORS -----
#define SHADER_BEGIN(NAME, WIDTH, HEIGHT) \
    CShader##NAME##::~CShader##NAME##() { \
        glDeleteProgram(m_program);
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
#define SHADER_UNIFORM(NAME)
#define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE) \
        glDeleteTextures(1, &m_texture_##NAME##);
#define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE) \
        glDeleteTextures(1, &m_texture_##NAME##);
#define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE) \
        glDeleteTextures(1, &m_texture_##NAME##);
#define SHADER_END() \
    } 
#include "ShaderDefs.h"

// ----- MAKE RENDER FUNCTIONS -----
#define SHADER_BEGIN(NAME, WIDTH, HEIGHT) \
    void CShader##NAME##::Render() { \
        glUseProgram(m_program); \
        GLuint texIndex = 0;
#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
        glBindBuffer(GL_ARRAY_BUFFER, m_attr_##NAME##_buffer); \
        glVertexAttribPointer(m_attr_##NAME##, ELEMENTSIZE, GLType<TYPE>(), false, 0, 0);
#define SHADER_UNIFORM(NAME) \
        glUniform1f(m_uniform_##NAME##, m_uniform_value_##NAME##);
#define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE) \
        glActiveTexture(GL_TEXTURE0 + texIndex); \
        glBindTexture(GL_TEXTURE_1D, m_texture_##NAME##); \
        glUniform1i(m_uniform_##NAME##, texIndex++);
#define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE) \
        glActiveTexture(GL_TEXTURE0 + texIndex); \
        glBindTexture(GL_TEXTURE_2D, m_texture_##NAME##); \
        glUniform1i(m_uniform_##NAME##, texIndex++);
#define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE) \
        glActiveTexture(GL_TEXTURE0 + texIndex); \
        glBindTexture(GL_TEXTURE_3D, m_texture_##NAME##); \
        glUniform1i(m_uniform_##NAME##, texIndex++);
#define SHADER_END() \
        glDrawArrays(GL_TRIANGLES, 0, m_numTriangles); \
    } 
#include "ShaderDefs.h"

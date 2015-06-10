//=============================================================================================================
// Expands ShaderDefs.h into CShader... classes
//=============================================================================================================

#include <vector>
#include "utils.h"

#define SHADER_BEGIN(NAME) \
    class CShader##NAME## { \
    public: \
        CShader##NAME##(); \
        ~CShader##NAME##(); \
        void Render(); \
        void Init(); \
        static const char *GetVertexShader(); \
        static const char *GetFragmentShader(); \

#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
    private: \
        GLuint m_attr_##NAME##; \
        GLuint m_attr_##NAME##_buffer; \
    public: \
        void SetAttributeData_##NAME##(const std::vector<TYPE>& data) { \
            m_attr_##NAME##_buffer = MakeBuffer<TYPE>(data); \
            m_numTriangles = data.size() / ELEMENTSIZE; \
        }

#define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE) \
    private: \
        GLuint m_uniform_##NAME##; \
        GLuint m_texture_##NAME##; \
    public: \
        void SetTextureData_##NAME##(GLsizei width, GLsizei height, const std::vector<TYPE>& data) { \
            m_texture_##NAME## = MakeTexture<TYPE>(width, height, data); \
        }

#define SHADER_END() \
    private: \
        GLuint m_program; \
        GLuint m_numTriangles; \
    };

#include "ShaderDefs.h"

// shader enum
enum EShaderTest {
    #define SHADER_BEGIN(NAME) e_shader##NAME,
    #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
    #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE)
    #define SHADER_END()
    #include "ShaderDefs.h"

    e_shaderInvalid,
    e_shaderFirst = 0,
    e_shaderLast = e_shaderInvalid - 1
};

// used by shader source code
#define SHADER_SOURCE(...) "#version 450\n" #__VA_ARGS__
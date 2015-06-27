//=============================================================================================================
// Expands ShaderDefs.h into CShader... classes
//=============================================================================================================

#include <vector>
#include "utils.h"

#define SHADER_BEGIN(NAME, WIDTH, HEIGHT) \
    class CShader##NAME## { \
    public: \
        CShader##NAME##(); \
        ~CShader##NAME##(); \
        void Render(); \
        void Init(); \
        static const char *GetName() { return #NAME; } \
        static const char *GetVertexShader(); \
        static const char *GetFragmentShader(); \
        static void GetWidthHeight(int &width, int &height) {width=WIDTH; height=HEIGHT;}

#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE) \
    private: \
        GLuint m_attr_##NAME##; \
        GLuint m_attr_##NAME##_buffer; \
    public: \
        void SetAttributeData_##NAME##(const std::vector<TYPE>& data) { \
            m_attr_##NAME##_buffer = MakeBuffer<TYPE>(data); \
            m_numTriangles = data.size() / ELEMENTSIZE; \
        }

#define SHADER_UNIFORM(NAME) \
    private: \
        GLuint m_uniform_##NAME##; \
        float m_uniform_value_##NAME##; \
    public: \
        void SetUniformData_##NAME##(float value) {m_uniform_value_##NAME## = value;} 

#define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP) \
    private: \
        GLuint m_uniform_##NAME##; \
        GLuint m_texture_##NAME##; \
    public: \
        void SetTextureData_##NAME##(GLsizei width, const std::vector<TYPE>& data) { \
            m_texture_##NAME## = MakeTexture1D<TYPE>(width, LERP, data); \
        }

#define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP) \
    private: \
        GLuint m_uniform_##NAME##; \
        GLuint m_texture_##NAME##; \
    public: \
        void SetTextureData_##NAME##(GLsizei width, GLsizei height, const std::vector<TYPE>& data) { \
            m_texture_##NAME## = MakeTexture2D<TYPE>(width, height, LERP, data); \
        }

#define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP) \
    private: \
        GLuint m_uniform_##NAME##; \
        GLuint m_texture_##NAME##; \
    public: \
        void SetTextureData_##NAME##(GLsizei width, GLsizei height, GLsizei depth, const std::vector<TYPE>& data) { \
            m_texture_##NAME## = MakeTexture3D<TYPE>(width, height, depth, LERP, data); \
        }

#define SHADER_END() \
    private: \
        GLuint m_program; \
        GLuint m_numTriangles; \
    };

#include "ShaderDefs.h"

// shader enum
enum EShaderTest {
    #define SHADER_BEGIN(NAME, WIDTH, HEIGHT) e_shader##NAME,
    #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
    #define SHADER_UNIFORM(NAME)
    #define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
    #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
    #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
    #define SHADER_END()
    #include "ShaderDefs.h"

    e_shaderInvalid,
    e_shaderFirst = 0,
    e_shaderLast = e_shaderInvalid - 1
};

// used by shader source code
#define SHADER_SOURCE(...) "#version 450\n" #__VA_ARGS__
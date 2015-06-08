/*
    Expands ShaderDefs.h into classes

*/

#include <vector>
#include "utils.h"

// TODO: remove friendship with main!

#define SHADER_BEGIN(NAME) \
    class CShader##NAME## { \
    public: \
        friend int main(void); \
        CShader##NAME##(); \
        ~CShader##NAME##(); \
        void Render(); \

#define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE) \
    private: \
        GLuint m_attr_##NAME##; \
        GLuint m_attr_##NAME##_buffer; \
    public: \
        void SetAttributeData_##NAME##(const std::vector<float>& data) { \
            m_attr_##NAME##_buffer = MakeFloatBuffer(data); \
            m_numTriangles = data.size() / ELEMENTSIZE; \
        }

#define SHADER_UNIFORM(NAME) \
    private: \
        GLuint m_uniform_##NAME##;

#define SHADER_TEXTURE(NAME) \
    private: \
        GLuint m_texture_##NAME##; \
    public: \
        void SetTextureData_##NAME##(GLsizei width, GLsizei height, const std::vector<unsigned char>& data) { \
            m_texture_##NAME## = MakeTexture(width, height, data); \
        }

#define SHADER_END() \
    private: \
        GLuint m_program; \
        GLuint m_numTriangles; \
    };

#include "ShaderDefs.h"
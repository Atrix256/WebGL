//=============================================================================================================
//    Macros to declare shaders
//
//    SHADER_BEGIN(NAME)
//        SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
//        SHADER_UNIFORM_TEXTURE(NAME, TYPE)
//    SHADER_END()
//
//    SHADER_BEGIN(NAME)
//
//    Defines a new shader class CShader<NAME>
//
//    SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
//
//    Defines a vertex attribute.  NAME is the name of the attribute as defined in the shader.  ELEMENTSIZE is
//    how many floats per element there are (a vec2 would be 2, for instance!).  Use SetAttributeData_<NAME>
//    to set data for this attribute.
//
//    TYPE can be float or double.
//
//    SHADER_UNIFORM_TEXTURE(NAME, TYPE)
//
//    Defines a shader uniform.  Also defines a texture to assign to it.  NAME is the name of the uniform as
//    defined in the shader.  Call SetTextureData_<NAME> to set the texture data.
//
//    Type can be float or unsigned char, but for now, always pass 0-255
//
//    SHADER_END()       - ends the shader declaration
//
//=============================================================================================================

SHADER_BEGIN(BilinearTest)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE(uSampler, float)
SHADER_END()

#undef SHADER_BEGIN
#undef SHADER_VERTEX_ATTRIBUTE
#undef SHADER_UNIFORM_TEXTURE
#undef SHADER_END
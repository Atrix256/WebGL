//=============================================================================================================
//    Macros to declare shaders
//
//    SHADER_BEGIN(NAME)
//        SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE)
//        SHADER_UNIFORM(NAME)
//        SHADER_TEXTURE(NAME)
//    SHADER_END()
//
//    SHADER_BEGIN(NAME)
//
//    Defines a new shader class CShader<NAME>
//
//    SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE)
//
//    Defines a vertex attribute.  NAME is the name of the attribute as defined in the shader.  ELEMENTSIZE is
//    how many floats per element there are (a vec2 would be 2, for instance!).  Use SetAttributeData_<NAME>
//    to set data for this attribute
//
//    SHADER_UNIFORM(NAME)
//
//    Defines a shader uniform.  NAME is the name of the uniform as defined in the shader.
//
//    SHADER_TEXTURE(NAME)
//
//    Defines a texture.  Call SetTextureData_<NAME> to set the texture data.  NAME is the name of the uniform
//    sampler to use this texture with.
//
//    SHADER_END()       - ends the shader declaration
//
//=============================================================================================================

SHADER_BEGIN(BilinearTest)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2)
    SHADER_UNIFORM(uSampler)
    SHADER_TEXTURE(uSampler)
SHADER_END()

#undef SHADER_BEGIN
#undef SHADER_VERTEX_ATTRIBUTE
#undef SHADER_UNIFORM
#undef SHADER_TEXTURE
#undef SHADER_END
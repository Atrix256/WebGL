//=============================================================================================================
//    Macros to declare shaders
//
//    SHADER_BEGIN(NAME, WIDTH, HEIGHT)
//        SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
//        SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
//        SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
//        SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
//        SHADER_UNIFORM(NAME)
//    SHADER_END()
//
//    SHADER_BEGIN(NAME, WIDTH, HEIGHT)
//
//    Defines a new shader class CShader<NAME>, and specifies the dimensions that it wants to run at
//
//    SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
//
//    Defines a vertex attribute.  NAME is the name of the attribute as defined in the shader.  ELEMENTSIZE is
//    how many floats per element there are (a vec2 would be 2, for instance!).  Use SetAttributeData_<NAME>
//    to set data for this attribute.
//
//    TYPE can be float or double.
//
//    SHADER_UNIFORM(NAME)
//
//    Lets you set a uniform in the shader.  NAME is the name of the uniform.  Assumed to be a float!
//
//    SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
//    SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
//    SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
//
//    Defines a shader uniform.  Also defines a texture to assign to it.  NAME is the name of the uniform as
//    defined in the shader.  Call SetTextureData_<NAME> to set the texture data. If LERP is true, it uses
//    linear sampling, else uses nearest.
//
//    Type can be float or unsigned char. floats should be 0..1, unsigned char should be 0..255
//
//    SHADER_END()       - ends the shader declaration
//
//=============================================================================================================

SHADER_BEGIN(BilinearTest, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(TrilinearTest, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(BilinearTest2, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(TrilinearTest2, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(BilinearTest3, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, false)
SHADER_END()

SHADER_BEGIN(TrilinearTest3, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, false)
SHADER_END()

SHADER_BEGIN(BilinearBakeTest, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
    SHADER_UNIFORM_TEXTURE_2D(uBakedSampler, float, true)
    SHADER_UNIFORM(uBakeSamples)
SHADER_END()

SHADER_BEGIN(TrilinearBakeTest, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
    SHADER_UNIFORM_TEXTURE_2D(uBakedSampler, float, true)
    SHADER_UNIFORM(uBakeSamples)
SHADER_END()

SHADER_BEGIN(RationalTest, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Quadratic2d, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Cubic2d, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Cubic3d, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Quartic2d, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Quartic3d, 1000, 500)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Texture1d, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 1, double)
    SHADER_UNIFORM_TEXTURE_1D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Texture2d, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
    SHADER_UNIFORM_TEXTURE_2D(uSampler, float, true)
SHADER_END()

SHADER_BEGIN(Texture3d, 1000, 1000)
    SHADER_VERTEX_ATTRIBUTE(aVertexPosition, 2, double)
    SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 3, double)
    SHADER_UNIFORM_TEXTURE_3D(uSampler, float, true)
SHADER_END()

#undef SHADER_BEGIN
#undef SHADER_VERTEX_ATTRIBUTE
#undef SHADER_UNIFORM
#undef SHADER_UNIFORM_TEXTURE_1D
#undef SHADER_UNIFORM_TEXTURE_2D
#undef SHADER_UNIFORM_TEXTURE_3D
#undef SHADER_END
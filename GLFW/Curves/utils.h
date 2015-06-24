//=============================================================================================================
// Utility functions
//=============================================================================================================

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <vector>

GLuint LoadShader(const char *vertexShader, const char *fragmentShader);

template<typename T>
GLuint MakeBuffer(const std::vector<T>& data);

template<typename T>
GLuint MakeTexture1D(GLsizei width, bool lerpSample, const std::vector<T>& data);

template<typename T>
GLuint MakeTexture2D(GLsizei width, GLsizei height, bool lerpSample, const std::vector<T>& data);

template<typename T>
GLuint MakeTexture3D(GLsizei width, GLsizei height, GLsizei depth, bool lerpSample, const std::vector<T>& data);

//=============================================================================================================
// Template Specializations
//=============================================================================================================

template<>
GLuint MakeBuffer<double>(const std::vector<double>& data);
template<>
GLuint MakeBuffer<float>(const std::vector<float>& data);

template<>
GLuint MakeTexture1D<unsigned char>(GLsizei width, bool lerpSample, const std::vector<unsigned char>& data);
template<>
GLuint MakeTexture1D<float>(GLsizei width, bool lerpSample, const std::vector<float>& data);

template<>
GLuint MakeTexture2D<unsigned char>(GLsizei width, GLsizei height, bool lerpSample, const std::vector<unsigned char>& data);
template<>
GLuint MakeTexture2D<float>(GLsizei width, GLsizei height, bool lerpSample, const std::vector<float>& data);

template<>
GLuint MakeTexture3D<unsigned char>(GLsizei width, GLsizei height, GLsizei depth, bool lerpSample, const std::vector<unsigned char>& data);
template<>
GLuint MakeTexture3D<float>(GLsizei width, GLsizei height, GLsizei depth, bool lerpSample, const std::vector<float>& data);


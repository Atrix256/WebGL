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
GLuint MakeTexture(GLsizei width, GLsizei height, const std::vector<T>& data);

//=============================================================================================================
// Template Specializations
//=============================================================================================================

template<>
GLuint MakeTexture<unsigned char>(GLsizei width, GLsizei height, const std::vector<unsigned char>& data);
template<>
GLuint MakeTexture<float>(GLsizei width, GLsizei height, const std::vector<float>& data);

template<>
GLuint MakeBuffer<double>(const std::vector<double>& data);
template<>
GLuint MakeBuffer<float>(const std::vector<float>& data);
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <vector>

GLuint LoadShader(const char *vertexShader, const char *fragmentShader);
GLuint MakeFloatBuffer(const std::vector<float>& data);
GLuint MakeTexture(GLsizei width, GLsizei height, const std::vector<unsigned char>& data);
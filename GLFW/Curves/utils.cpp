//=============================================================================================================
// Utility functions
//=============================================================================================================

#include "utils.h"
#include <stdio.h>
#include <vector>
#include <algorithm>

//=============================================================================================================
GLuint LoadShader(const char *vertexShader, const char *fragmentShader) {
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    printf("Compiling vertex shader.\n");
    glShaderSource(vertShader, 1, &vertexShader, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
    printf("%s\n", &vertShaderError[0]);

    // Compile fragment shader
    printf("Compiling fragment shader.\n");
    glShaderSource(fragShader, 1, &fragmentShader, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
    printf("%s\n", &fragShaderError[0]);

    printf("Linking program\n");
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
    printf("%s\n", &programError[0]);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

//=============================================================================================================
template<>
GLuint MakeBuffer<double>(const std::vector<double>& data)
{
    GLuint buffer = 0;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(double), &data[0], GL_STATIC_DRAW);
    return buffer;
}

//=============================================================================================================
template<>
GLuint MakeBuffer<float>(const std::vector<float>& data)
{
    GLuint buffer = 0;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    return buffer;
}

//=============================================================================================================
template<>
GLuint MakeTexture1D<unsigned char>(GLsizei width, const std::vector<unsigned char>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_1D, 1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, NULL);
    return texture;
}

//=============================================================================================================
template<>
GLuint MakeTexture1D<float>(GLsizei width, const std::vector<float>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_1D, 1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, width, 0, GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_1D, NULL);
    return texture;
}

//=============================================================================================================
template<>
GLuint MakeTexture2D<unsigned char>(GLsizei width, GLsizei height, const std::vector<unsigned char>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, NULL);
    return texture;
}

//=============================================================================================================
template<>
GLuint MakeTexture2D<float>(GLsizei width, GLsizei height, const std::vector<float>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, NULL);
    return texture;
}

//=============================================================================================================
template<>
GLuint MakeTexture3D<unsigned char>(GLsizei width, GLsizei height, GLsizei depth, const std::vector<unsigned char>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_3D, 1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, NULL);
    return texture;
}

//=============================================================================================================
template<>
GLuint MakeTexture3D<float>(GLsizei width, GLsizei height, GLsizei depth, const std::vector<float>& data)
{
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_3D, 1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, NULL);
    return texture;
}

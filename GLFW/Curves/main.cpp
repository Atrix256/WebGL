#include "GL/glew.h"
#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Shaders.inl"

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

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint MakeVertexBuffer()
{
    static const float verts[] =
    {
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        -1.0, -1.0,
        1.0, 1.0,
        -1.0, 1.0,
    };

    GLuint buffer = 0;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    return buffer;
}

GLuint MakeTextureCoordinateBuffer()
{
    static const float coords[] =
    {
        0.0, 0.0,
        2.0, 0.0,
        2.0, 1.0,
        0.0, 0.0,
        2.0, 1.0,
        0.0, 1.0,
    };

    GLuint buffer = 0;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);

    return buffer;
}

GLuint MakeTexture()
{
    static const unsigned char textureData[] =
    {
        128, 0, 0, 255,      50, 0, 0, 255,
         50, 0, 0, 255,     200, 0, 0, 255
    };    GLsizei width = 2;    GLsizei height = 2;
    GLuint texture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, NULL);
    return texture;
}

int main(void)
{
    printf(s_vertexShader);
    //FreeConsole();
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    GLuint program = LoadShader(s_vertexShader, s_fragmentShader);
    glUseProgram(program);

    GLuint vertexBuffer = MakeVertexBuffer();
    GLuint textureCoordinateBuffer = MakeTextureCoordinateBuffer();
    GLuint theTexture = MakeTexture();

    GLuint vertexPositionAttribute = glGetAttribLocation(program, "aVertexPosition");
    glEnableVertexAttribArray(vertexPositionAttribute);

    GLuint textureCoordinateAttribute = glGetAttribLocation(program, "aTextureCoord");
    glEnableVertexAttribArray(textureCoordinateAttribute);

    GLuint samplerUniform = glGetUniformLocation(program, "uSampler");

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        #if 0
            glBegin(GL_TRIANGLES);

            glVertex3f(-1.0f, -1.0f, 0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);

            glVertex3f(-1.0f, -1.0f, 0.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);

            glEnd();
        #else        
            glUseProgram(program);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glVertexAttribPointer(vertexPositionAttribute, 2, GL_FLOAT, false, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
            glVertexAttribPointer(textureCoordinateAttribute, 2, GL_FLOAT, false, 0, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, theTexture);
            glUniform1i(samplerUniform, 0);
            glDrawArrays(GL_TRIANGLES,0,6);
        #endif

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
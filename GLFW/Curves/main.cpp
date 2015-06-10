//=============================================================================================================
// Main
//=============================================================================================================

/*

TODO:
* changing this to float breaks it! SHADER_VERTEX_ATTRIBUTE(aTextureCoord, 2, double)
*/

#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Shaders.h"

//=============================================================================================================
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

//=============================================================================================================
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

//=============================================================================================================
int main(void)
{
    //FreeConsole();
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(1000, 500, "", NULL, NULL);
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
        printf("Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));

    printf("Using OpenGL %s\n", glGetString(GL_VERSION));

    // create the shader and set up shader data
    CShaderBilinearTest shader;
    shader.SetAttributeData_aTextureCoord({
        0.0, 0.0,
        2.0, 0.0,
        2.0, 1.0,
        0.0, 0.0,
        2.0, 1.0,
        0.0, 1.0,
    });
    shader.SetAttributeData_aVertexPosition({
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
        -1.0, -1.0,
         1.0,  1.0,
        -1.0,  1.0,
    });

    shader.SetTextureData_uSampler(2, 2, {
        128, 0, 0, 255,      50, 0, 0, 255,
         50, 0, 0, 255,     200, 0, 0, 255
    });

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
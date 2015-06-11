//=============================================================================================================
// Main
//=============================================================================================================

/*

TODO:

* render control points in bilinear and trilinear test

* finish Shader_Quadratic2d
* make a higher order 2d texture curve
* make a shader_cubic3d
* make a higher order 3d texture curve
* can we use vec2d etc? i think not (says it needs to be flat?)... keep trying to fight the precision problems though if you can.

* render control points for all tests!
*/

#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Shaders.h"

EShaderTest g_whichTest = EShaderTest::e_shaderFirst;

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

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if (g_whichTest > EShaderTest::e_shaderFirst)
            g_whichTest = EShaderTest(g_whichTest - 1);
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if (g_whichTest < EShaderTest::e_shaderLast)
            g_whichTest = EShaderTest(g_whichTest + 1);
    }
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

    // init our shaders
    #define SHADER_BEGIN(NAME) CShader##NAME shader##NAME##;  shader##NAME##.Init();
    #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
    #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE)
    #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE)
    #define SHADER_END()
    #include "ShaderDefs.h"

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the right shader
        switch (g_whichTest){
        #define SHADER_BEGIN(NAME) case e_shader##NAME: shader##NAME##.Render(); break;
        #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
        #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE)
        #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE)
        #define SHADER_END()
        #include "ShaderDefs.h"
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
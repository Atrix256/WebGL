//=============================================================================================================
// Main
//=============================================================================================================

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
    glfwSwapInterval(0);
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
    #define SHADER_BEGIN(NAME, WIDTH, HEIGHT) CShader##NAME shader##NAME##;  shader##NAME##.Init();
    #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
    #define SHADER_UNIFORM(NAME)
    #define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
    #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
    #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
    #define SHADER_END()
    #include "ShaderDefs.h"

    // render loop
    char FPS[64];
    sprintf(FPS, "(FPS: -- / ms: --)");
    int frameCount = 0;
    double lastFPSTime = glfwGetTime();
    EShaderTest lastTest = e_shaderInvalid;
    bool updateTitle = false;
    while (!glfwWindowShouldClose(window))
    {

        // if we changed which test we are viewing, we need to make sure the title gets updated, and also to change the window size if needed
        
        if (g_whichTest != lastTest) {
            lastTest = g_whichTest;
            updateTitle = true;

            int width, height;
            switch (g_whichTest){
            #define SHADER_BEGIN(NAME, WIDTH, HEIGHT) case e_shader##NAME: shader##NAME##.GetWidthHeight(width,height); break;
            #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
            #define SHADER_UNIFORM(NAME)
            #define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
            #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
            #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
            #define SHADER_END()
            #include "ShaderDefs.h"
            }
            glfwSetWindowSize(window, width, height);
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        char title[256];
        ++frameCount;

        double currentTime = glfwGetTime();
        if (currentTime - lastFPSTime > 1.0) {
            double elapsed = currentTime - lastFPSTime;
            float fps = float(frameCount) / float(elapsed);
            sprintf(FPS, "(FPS: %0.1f / ms: %0.4f", fps, 1000.0 / fps);
            lastFPSTime = currentTime;
            frameCount = 0;
            updateTitle = true;
        }

        // update the title bar if we should
        if (updateTitle) {
            switch (g_whichTest){
            #define SHADER_BEGIN(NAME, WIDTH, HEIGHT) case e_shader##NAME: sprintf(title,"%s %s",#NAME " - Arrow keys to cycle test", FPS);glfwSetWindowTitle(window, title); break;
            #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
            #define SHADER_UNIFORM(NAME)
            #define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
            #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
            #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
            #define SHADER_END()
            #include "ShaderDefs.h"
            }
            updateTitle = false;
        }

        // render the right shader
        switch (g_whichTest){
        #define SHADER_BEGIN(NAME, WIDTH, HEIGHT) case e_shader##NAME: shader##NAME##.Render(); break;
        #define SHADER_VERTEX_ATTRIBUTE(NAME, ELEMENTSIZE, TYPE)
        #define SHADER_UNIFORM(NAME)
        #define SHADER_UNIFORM_TEXTURE_1D(NAME, TYPE, LERP)
        #define SHADER_UNIFORM_TEXTURE_2D(NAME, TYPE, LERP)
        #define SHADER_UNIFORM_TEXTURE_3D(NAME, TYPE, LERP)
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
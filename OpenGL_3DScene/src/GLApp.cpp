// STD includes
#include <iostream>

// GLEW
#include "GL/glew.h"

// My includes
#include "GLApp.h"
#include "GLDebug.h"

GLFWwindow* GLApp::window = nullptr;
GLFWmonitor* GLApp::monitor = nullptr;
const GLFWvidmode* GLApp::vmode = nullptr;

bool GLApp::init(GLFWwindow** _window, const char* name /*= GLFW Window*/)
{
    /* Инициализация библиотеки GLFW */
    if (!glfwInit())
    {
        std::cout << "Error initializing glfw lib" << std::endl;
        return false;
    }

    /* Использование версии OpenGL 4.3 CORE */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Создание окна, задаём размеры, название */
    *_window = glfwCreateWindow(640, 480, name, NULL, NULL);
    if (!_window)
    {
        std::cout << "Error creating window." << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(*_window);

    glfwSwapInterval(1); // VSYNC

    /* (!) glewInit() должен вызываться после полного создания окна
            при помощи библиотеки GLFW */
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initializing GLEW" << std::endl;
        return false;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
        << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl
        << "OpenGL Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    window = *_window;

    return true;
}


bool GLApp::init(GLFWwindow** _window, GLFWmonitor** _monitor, const GLFWvidmode** _vmode, const char* name /*= GLFW Window */)
{
    /* Инициализация библиотеки GLFW */
    if (!glfwInit())
    {
        std::cout << "Error initializing glfw lib" << std::endl;
        return false;
    }

    /* Использование версии OpenGL 4.3 CORE */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Создание окна, задаём размеры, название */
    *_monitor = glfwGetPrimaryMonitor();
    *_vmode = glfwGetVideoMode(*_monitor);
	*_window = glfwCreateWindow((*_vmode)->width, (*_vmode)->height, name, NULL, NULL);
    if (!_window)
    {
        std::cout << "Error creating window." << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(*_window);

    glfwSwapInterval(1); // VSYNC

    /* (!) glewInit() должен вызываться после полного создания окна
            при помощи библиотеки GLFW */
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initializing GLEW" << std::endl;
        return false;
    }
    
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
        << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl
        << "OpenGL Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    window = *_window;
    monitor = *_monitor;
    vmode = *_vmode;

    return true;
}

int& GLApp::getWindowWidth()
{
    int windowWidth = vmode->width;
    return windowWidth;
}

int& GLApp::getWindowHeight()
{
    int windowHeight = vmode->height;
    return windowHeight;
}

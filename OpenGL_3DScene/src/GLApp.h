#pragma once

// GLFW
#include "GLFW/glfw3.h"

class GLApp 
{
private:
	static GLFWwindow* window;
	static GLFWmonitor* monitor;
	const static GLFWvidmode* vmode;

public:
	static const int monitorLeftCornerX = 0;
	static const int monitorLeftCornerY = 0;

	static bool init(GLFWwindow** window, const char* name = "GLFW window");
	static bool init(GLFWwindow** window, GLFWmonitor** monitor, const GLFWvidmode** vmode, const char* name = "GLFW Window");
	static inline void terminate() { glfwTerminate(); }

	static int& getWindowWidth();
	static int& getWindowHeight();

	static inline GLFWwindow* getWindow() { return window; };
	static inline GLFWmonitor* getMonitor() { return monitor; };
	static inline const GLFWvidmode* getVmode() { return vmode; };
};
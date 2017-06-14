#include "..\Headers\Render.h"

GLFWwindow* CreateWindow(int Width, int Height, const char* Title, GLFWmonitor* monitor, GLFWwindow* share){
	//Create window using OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//Make window resizable
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//MSAAx8 enabled
	//glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow *window = glfwCreateWindow(Width, Height, Title, monitor, share);
	return window;
}
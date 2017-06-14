#include "..\Headers\Render.h"
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint CompileShaders(const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource);

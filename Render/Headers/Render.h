#pragma once
#include "OpenGLincludes.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <cstring>
#include <string>
#include <mutex>
#define degreesToRadians(x) x*(3.141592f/180.0f)
typedef struct Vertex {
	GLfloat position[3];
}Vertex;
typedef struct Vertex2d{
	GLfloat position[2];
}Vertex2d;

typedef struct texCoord {
	GLfloat texCoordinate[2];
}texCoord;

typedef struct material {
	GLfloat ka;
	GLfloat kd;
	GLfloat ks;
	GLfloat shininess;
}material;
typedef struct Light {
	glm::vec3 position;
	glm::vec3 intensity;
}Light;

GLuint CompileShaders(const char *vertexShaderSource, const char *fragmentShaderSource);

GLuint CompileComputeShader(const char *computeShaderSource);

GLFWwindow* CreateWindow(int Width, int Height, const char* Title, GLFWmonitor* monitor, GLFWwindow* share);
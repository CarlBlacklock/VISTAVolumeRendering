#include "..\Headers\marchingCubes.h"

void marchCubes(float threshold, int *numberOfVerticesGenerated, GLuint *VAO) {
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);

}
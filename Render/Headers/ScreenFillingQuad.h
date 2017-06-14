#pragma once
#include "..\Headers\Render.h"

class ScreenFillingQuad {
	Vertex *vertices;
	texCoord *texCoords;
	GLuint VAO;
	GLuint VBO[3];
public:
	ScreenFillingQuad();
	void CleanUp();
	void DrawScreenFillingQuad(GLuint program, glm::mat4 FrustrumMatrix, glm::mat4 ViewMatrix, GLuint texture);


};
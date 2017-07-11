#pragma once
#include "..\Headers\Render.h"
class Histogram{
	Vertex2d *vertices;
	GLuint VAO;
	GLuint VBO[2];
	int totalSubdivisions;
public:
	Histogram(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles);
	void CleanUp();
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	~Histogram();
};


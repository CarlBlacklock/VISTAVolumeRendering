#pragma once
#include "..\Headers\Render.h"
class Histogram{
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO[3];
	int totalSubdivisions;
	int xRes, yRes, numFiles;
public:
	Histogram(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles);
	Histogram(glm::vec3 focus, int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram);
	void ChangeFocus(glm::vec3 focus, GLuint histoProgram, unsigned char* volumeData);
	void CleanUp();
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	~Histogram();
};


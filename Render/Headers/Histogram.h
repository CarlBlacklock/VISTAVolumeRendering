#pragma once
#include "..\Headers\Render.h"
class Histogram{
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO[3];
	int totalSubdivisions;
	int xRes, yRes, numFiles;
public:
	Histogram();
	Histogram(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles);
	Histogram(glm::vec3 focus, int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles);
	Histogram(glm::vec3 focus, int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram, GLuint filteredVolume);
	void ChangeFocus(glm::vec3 focus,  unsigned char* volumeData);
	void CleanUp();
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	~Histogram();
};


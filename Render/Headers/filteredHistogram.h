#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\CompileShaders.h"
class filteredHistogram
{
private:
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO[3];
	GLuint countsTexture;
	GLuint scaledCountsTexture;
	int totalSubdivisions;
	int xRes, yRes, numFiles;
public:
	filteredHistogram();
	filteredHistogram(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram, unsigned char* volumeData);
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	void CleanUp();
	~filteredHistogram();
};


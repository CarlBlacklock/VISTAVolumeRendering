#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\CompileShaders.h"
class probeFilteredHistogram
{
private:
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO[3];
	GLuint countsTexture;
	GLuint scaledCountsTexture;
	GLuint filteredVolume;
	GLuint myProgram;
	int totalSubdivisions;
	int xRes, yRes, numFiles;
	glm::vec3 myFocus;
	unsigned char* dataLocation;
	void calculateHistogram(GLuint histoProgram);
public:
	probeFilteredHistogram();
	probeFilteredHistogram(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram, unsigned char* volumeData, glm::vec3 focus);
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	void CleanUp();
	void ChangeFocus(glm::vec3 newFocus);
	~probeFilteredHistogram();
};


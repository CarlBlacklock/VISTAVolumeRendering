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
	GLuint scalingProgram;
	int totalSubdivisions;
	int xRes, yRes, numFiles;
	glm::ivec3 myFocus;
	unsigned char* dataLocation;
	void calculateHistogram(GLuint histoProgram);
	void recalculateHistogram(GLuint histoProgram);
public:
	probeFilteredHistogram();
	probeFilteredHistogram(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram, unsigned char* volumeData, glm::ivec3 focus);
	void DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix);
	void CleanUp();
	void ChangeFocus(glm::ivec3 newFocus);
	~probeFilteredHistogram();
};


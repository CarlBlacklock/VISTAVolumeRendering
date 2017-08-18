#pragma once
#include "..\Headers\Render.h"
class DataCube{
	Vertex *vertices;
	Vertex *texCoords;
	GLuint VAO;
	GLuint VBO[3];
	glm::mat4 ModelMatrix;
public:
	DataCube();
	void CleanUp();
	void DrawDataCube(GLuint program, GLuint volumeData);
	void DrawDataCubeOrthoView(GLuint program, GLuint volumeData, glm::mat4 OrthoMatrix, glm::mat4 ViewMatrix, glm::vec3 ViewingDir, glm::vec3 Resolution, float *extents, float alpha, float beta, int filterMode);
	void DrawDataCubeWithGaussian(GLuint program, GLuint volumeData, glm::mat4 OrthoMatrix, glm::mat4 ViewMatrix, glm::vec3 ViewingDir, glm::vec3 Resolution, float *extents, float alpha, float beta, int filterMode, float a, float b, float c);
	void MultiplyModelMatrix(glm::mat4 Transformation);
	void ResetModelMatrix();
	~DataCube();
};


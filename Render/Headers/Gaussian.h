#pragma once
#include "../Headers/Render.h"
class Gaussian
{
public:
	Gaussian();
	Gaussian(int k);
	~Gaussian();
	void CleanUp();
	float getA();
	float getB();
	float getC();
	void addA(float newA);
	void addB(float newB);
	void addC(float newC);
	void DrawGaussian(GLuint program, glm::mat4 ProjectionMatrix);

private:
	float a, b, c;
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO;
};


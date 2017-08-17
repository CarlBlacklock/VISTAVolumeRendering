#pragma once
#include "../Headers/Render.h"
class Gaussian
{
public:
	Gaussian();
	Gaussian(int k);
	~Gaussian();
	float getA();
	float getB();
	float getC();
	void setA();
	void setB();
	void setC();
	void DrawGaussian(GLuint program, glm::mat4 ProjectionMatrix);

private:
	float a, b, c;
	Vertex *vertices;
	GLuint VAO;
	GLuint VBO;
};


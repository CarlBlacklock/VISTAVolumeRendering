#pragma once
#include "..\Headers\Render.h"

void calculateGradients(GLuint gradientProgram, int xResolution, int yResolution, int numberOfFiles, GLuint* gradientID);

void sobelGaussFilter(GLuint sobelGaussFilterProgram, int xResolution, int yResolution, int numberOfFiles, GLuint* sobelGaussID);

void marchCubes(float threshold, int *numberOfVerticesGenerated, GLuint *VAO, int xResolution, int yResolution, int numberOfFiles);

#pragma once
#include "..\Headers\Render.h"

void calculateGradients(GLuint gradientProgram, int xResolution, int yResolution, int numberOfFiles, GLuint* gradientID);

void marchCubes(float threshold, int *numberOfVerticesGenerated, GLuint *VAO, int xResolution, int yResolution, int numberOfFiles);
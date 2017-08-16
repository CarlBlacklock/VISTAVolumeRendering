#include "..\Headers\probeFilteredHistogram.h"



void probeFilteredHistogram::calculateHistogram(GLuint histoProgram){
	GLenum errorType;
	GLuint volumeID;
	glGenTextures(1, &volumeID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, xRes, yRes, numFiles, 0, GL_RED, GL_UNSIGNED_BYTE, dataLocation);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	GLuint sobelGaussFilterProgram = CompileComputeShader("../Shaders/sobelGaussFilter.comp");
	sobelGaussFilter(sobelGaussFilterProgram, xRes, yRes, numFiles, &filteredVolume);

	int* initialCounts = (int *)malloc(totalSubdivisions * sizeof(int));
	int i;
	for (i = 0; i < totalSubdivisions; i++) {
		initialCounts[i] = 0;
	}

	glGenTextures(1, &countsTexture);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_1D, countsTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, totalSubdivisions, 0, GL_RED_INTEGER, GL_INT, initialCounts);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glBindImageTexture(3, countsTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_3D, filteredVolume);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glUseProgram(histoProgram);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	GLint readLocation, writeLocation;
	readLocation = glGetUniformLocation(histoProgram, "TextureSampler");
	glUniform1i(readLocation, 4);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	GLint numLocation;
	numLocation = glGetUniformLocation(histoProgram, "numberOfSubdivisions");
	glUniform1i(numLocation, totalSubdivisions);

	GLint focusLocation;
	focusLocation = glGetUniformLocation(histoProgram, "focus");
	glUniform3fv(focusLocation, 1, glm::value_ptr(myFocus));
	glDispatchCompute(7, 7, 7);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindImageTexture(3, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	glFinish();

	GLfloat* initialScaling = (GLfloat *)malloc(totalSubdivisions * sizeof(GLfloat));
	for (i = 0; i < totalSubdivisions; i++) {
		initialScaling[i] = 0.0f;
	}
	glGenTextures(1, &scaledCountsTexture);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_1D, scaledCountsTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, totalSubdivisions, 0, GL_RED, GL_FLOAT, initialScaling);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glBindImageTexture(3, scaledCountsTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

	GLuint scalingProgram = CompileComputeShader("../Shaders/GlobalHistogramScaling.comp");
	glUseProgram(scalingProgram);

	readLocation = glGetUniformLocation(scalingProgram, "TextureSampler");
	glUniform1i(readLocation, 9);

	numLocation = glGetUniformLocation(scalingProgram, "numberOfSubdivisions");
	glUniform1i(numLocation, totalSubdivisions);

	glBindImageTexture(4, countsTexture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32I);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	glDispatchCompute(totalSubdivisions, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindImageTexture(3, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glFinish();

	glDeleteProgram(sobelGaussFilterProgram);
	glDeleteProgram(scalingProgram);
	glDeleteTextures(1, &volumeID);
	free(initialCounts);
	free(initialScaling);
}

probeFilteredHistogram::probeFilteredHistogram()
{
}

probeFilteredHistogram::probeFilteredHistogram(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram, unsigned char * volumeData, glm::vec3 focus){
	GLenum errorType;
	myProgram = histoProgram;
	myFocus = focus;
	totalSubdivisions = numberOfSubdivisions;
	dataLocation = volumeData;
	float barWidth = 1.0f / (float)totalSubdivisions;
	int i;
	vertices = (Vertex *)malloc(6 * sizeof(Vertex));
	if (vertices != NULL) {
		vertices[0].position[0] = 0.0f;
		vertices[0].position[1] = 0.0f;
		vertices[0].position[2] = 0.0f;

		vertices[1].position[0] = 1.0f;
		vertices[1].position[1] = 0.f;
		vertices[1].position[2] = 0.0f;

		vertices[2].position[0] = 1.0f;
		vertices[2].position[1] = 1.0f;
		vertices[2].position[2] = 0.0f;

		vertices[3].position[0] = 0.0f;
		vertices[3].position[1] = 0.0f;
		vertices[3].position[2] = 0.0f;

		vertices[4].position[0] = 1.0f;
		vertices[4].position[1] = 1.0f;
		vertices[4].position[2] = 0.0f;

		vertices[5].position[0] = 0.0f;
		vertices[5].position[1] = 1.0f;
		vertices[5].position[2] = 0.0f;

		glm::mat4 *modelMatrices;
		modelMatrices = (glm::mat4 *)malloc(totalSubdivisions * sizeof(glm::mat4));
		if (modelMatrices != NULL) {
			glm::mat4 identityMatrix = glm::mat4(1.0f);
			glm::mat4 scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, 1.0f, 0.0f));
			glm::mat4 translatePart = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			modelMatrices[0] = translatePart * scalePart * identityMatrix;
			for (i = 1; i < totalSubdivisions; i++) {
				scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, 1.0f, 0.0f));
				translatePart = glm::translate(identityMatrix, glm::vec3((float)i * barWidth, 0.0f, 0.0f));
				modelMatrices[i] = translatePart * scalePart * identityMatrix;
			}
			calculateHistogram(histoProgram);
			glGenBuffers(3, VBO);
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, totalSubdivisions * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(1);
			GLsizei vec4Size = sizeof(glm::vec4);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid *)0);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid *)(vec4Size));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid *)(2 * vec4Size));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid *)(3 * vec4Size));
			glVertexAttribDivisor(1, 1);
			glVertexAttribDivisor(2, 1);
			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glBindVertexArray(0);
			while ((errorType = glGetError()) != GL_NO_ERROR) {

			}
			//free(offsets);
			free(modelMatrices);
			free(vertices);
			//free(numberOfOccurrances);
			
			glDeleteTextures(1, &countsTexture);
			
		}
	}


}

void probeFilteredHistogram::DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix){
	GLenum errorType;
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glUseProgram(program);
	GLint orthoLocation;
	orthoLocation = glGetUniformLocation(program, "ProjectionMatrix");
	glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	GLint textureLocation;
	textureLocation = glGetUniformLocation(program, "counts");
	glUniform1i(textureLocation, 10);
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, totalSubdivisions);

	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	glBindVertexArray(0);
}


void probeFilteredHistogram::CleanUp(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(3, VBO);
	glDeleteTextures(1, &scaledCountsTexture);
	glDeleteTextures(1, &filteredVolume);
}

void probeFilteredHistogram::ChangeFocus(glm::vec3 newFocus){
	myFocus = newFocus;
	calculateHistogram(myProgram);
}

probeFilteredHistogram::~probeFilteredHistogram()
{
}

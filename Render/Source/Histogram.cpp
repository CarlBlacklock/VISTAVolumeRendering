#include "..\Headers\Histogram.h"
#include "..\Headers\CompileShaders.h"



Histogram::Histogram(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles){
	totalSubdivisions = numberOfSubdivisions;
	int *numberOfOccurrances;
	numberOfOccurrances = (int *) malloc(totalSubdivisions * sizeof(int));
	if (numberOfOccurrances != NULL) {

		int i;
		for (i = 0; i < totalSubdivisions; i++) {
			numberOfOccurrances[i] = 0;
		}
		int upperBound = 255 / totalSubdivisions;
		int totalSamples = xResolution * yResolution * numberOfFiles;
		int rangeFit = 0;
		for (i = 0; i < totalSamples; i++) {
			rangeFit = 0;
			int sampleValue = (int)volumeData[i];
			if (sampleValue > 0) {
				rangeFit = sampleValue / upperBound;
				if (rangeFit == totalSubdivisions) {
					rangeFit--;
				}
				numberOfOccurrances[rangeFit]++;
			}
		}
		int largestNumberOfOccurances = 0;
		for (i = 0; i < totalSubdivisions; i++) {
			if (numberOfOccurrances[i] > largestNumberOfOccurances) {
				largestNumberOfOccurances = numberOfOccurrances[i];
			}
		}
		float barWidth = 1.0f / (float)totalSubdivisions;
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
				float barHeight = (float)numberOfOccurrances[0] / (float)largestNumberOfOccurances;
				glm::mat4 scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
				glm::mat4 translatePart = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
				modelMatrices[0] = translatePart * scalePart * identityMatrix;
				for (i = 1; i < totalSubdivisions; i++) {
					barHeight = (float)numberOfOccurrances[i] / (float)largestNumberOfOccurances;
					scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
					translatePart = glm::translate(identityMatrix, glm::vec3((float)i * barWidth, 0.0f, 0.0f));
					modelMatrices[i] = translatePart * scalePart * identityMatrix;
				}

				


					glGenBuffers(2, VBO);
					glGenVertexArrays(1, &VAO);
					glBindVertexArray(VAO);
					glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
					glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
					glEnableVertexAttribArray(0);
					/*
					glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
					glBufferData(GL_ARRAY_BUFFER, totalSubdivisions * sizeof(GLfloat), offsets, GL_STATIC_DRAW);
					glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *)0);
					glEnableVertexAttribArray(1);
					*/
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
					glVertexAttribDivisor(5, 1);
					GLenum errorType;
					while ((errorType = glGetError()) != GL_NO_ERROR) {

					}


					glBindVertexArray(0);
					//free(offsets);
					free(modelMatrices);
					free(vertices);
					free(numberOfOccurrances);
				
			}
		}
	}

}

Histogram::Histogram(glm::vec3 focus, int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, GLuint histoProgram) {
	totalSubdivisions = numberOfSubdivisions;
	xRes = xResolution;
	yRes = yResolution;
	numFiles = numberOfFiles;
	int *numberOfOccurrances;
	numberOfOccurrances = (int *)malloc(totalSubdivisions * sizeof(int));
	if (numberOfOccurrances != NULL) {

		int i;
		for (i = 0; i < totalSubdivisions; i++) {
			numberOfOccurrances[i] = 0;
		}
		int upperBound = 255 / totalSubdivisions;
		glm::ivec3 intFocus = glm::ivec3((int)((float)xResolution * focus.x), (int)((float)yResolution * focus.y), (int)((float)numberOfFiles * focus.z));
		int rangeFit = 0;
		int k, j;
		for (i = -1; i < 2; i++) {
			for (k = -1; k < 2; k++) {
				for (j = -1; j < 2; j++) {
					rangeFit = 0;
					if (intFocus.z + i < 0 || intFocus.z + i > 224 || intFocus.y + k < 0 || intFocus.y + k > 255 || intFocus.x + j < 0 || intFocus.x + j > 255) {

					}
					else {
						int sampleValue = (int)volumeData[intFocus.x + i + 256 * (intFocus.y + k + 225 * intFocus.z + j)];
						if (sampleValue > 0) {
							rangeFit = sampleValue / upperBound;
							if (rangeFit == totalSubdivisions) {
								rangeFit--;
							}
							numberOfOccurrances[rangeFit]++;
						}
					}
				}
			}
		}
		/*
		GLuint histoTex;
		glGenTextures(1, &histoTex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_1D, histoTex);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, totalSubdivisions, 0, GL_RED, GL_INT, numberOfOccurrances);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glBindImageTexture(3, histoTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);

		glUseProgram(histoProgram);

		GLint location;
		location = glGetUniformLocation(histoProgram, "TextureSampler");
		glUniform1i(location, 0);

		location = glGetUniformLocation(histoProgram, "upperBound");
		glUniform1i(location, upperBound);

		location = glGetUniformLocation(histoProgram, "totalSubdivisions");
		glUniform1i(location, totalSubdivisions);

		location = glGetUniformLocation(histoProgram, "focus");
		glUniform3iv(location, 1, glm::value_ptr(intFocus));

		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glFinish();
		
		glGetTexImage(GL_TEXTURE_1D, 0, GL_RED, GL_INT, numberOfOccurrances);
		glFinish();
		*/
		int largestNumberOfOccurances = 0;
		for (i = 0; i < totalSubdivisions; i++) {
			if (numberOfOccurrances[i] > largestNumberOfOccurances) {
				largestNumberOfOccurances = numberOfOccurrances[i];
			}
		}

		float barWidth = 1.0f / (float)totalSubdivisions;
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
				float barHeight = (float)numberOfOccurrances[0] / (float)largestNumberOfOccurances;
				glm::mat4 scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
				glm::mat4 translatePart = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
				modelMatrices[0] = translatePart * scalePart * identityMatrix;
				for (i = 1; i < totalSubdivisions; i++) {
					barHeight = (float)numberOfOccurrances[i] / (float)largestNumberOfOccurances;
					scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
					translatePart = glm::translate(identityMatrix, glm::vec3((float)i * barWidth, 0.0f, 0.0f));
					modelMatrices[i] = translatePart * scalePart * identityMatrix;
				}
				glGenBuffers(2, VBO);
				glGenVertexArrays(1, &VAO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
				glEnableVertexAttribArray(0);
				/*
				glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
				glBufferData(GL_ARRAY_BUFFER, totalSubdivisions * sizeof(GLfloat), offsets, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *)0);
				glEnableVertexAttribArray(1);
				*/
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
				glVertexAttribDivisor(5, 1);
				GLenum errorType;
				while ((errorType = glGetError()) != GL_NO_ERROR) {

				}


				glBindVertexArray(0);
				//free(offsets);
				free(modelMatrices);
				free(vertices);
				free(numberOfOccurrances);
				//glDeleteTextures(1, &histoTex);
			}

		}


	}
}



void Histogram::ChangeFocus(glm::vec3 focus, GLuint histoProgram, unsigned char* volumeData) {
	
	int *numberOfOccurrances;
	numberOfOccurrances = (int *)malloc(totalSubdivisions * sizeof(int));
	if (numberOfOccurrances != NULL) {

		int i;
		for (i = 0; i < totalSubdivisions; i++) {
			numberOfOccurrances[i] = 0;
		}
		int upperBound = 255 / totalSubdivisions;
		glm::ivec3 intFocus = glm::ivec3((int)((float)xRes * focus.x), (int)((float)yRes * focus.y), (int)((float)numFiles * focus.z));
		/*
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLuint histoTex;
		glGenTextures(1, &histoTex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_1D, histoTex);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, totalSubdivisions, 0, GL_RED, GL_INT, numberOfOccurrances);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glBindImageTexture(3, histoTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32I);

		glUseProgram(histoProgram);

		GLint location;
		location = glGetUniformLocation(histoProgram, "TextureSampler");
		glUniform1i(location, 0);

		location = glGetUniformLocation(histoProgram, "upperBound");
		glUniform1i(location, upperBound);

		location = glGetUniformLocation(histoProgram, "totalSubdivisions");
		glUniform1i(location, totalSubdivisions);

		location = glGetUniformLocation(histoProgram, "focus");
		glUniform3iv(location, 1, glm::value_ptr(intFocus));

		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glFinish();
		GLenum errorType;
		while ((errorType = glGetError()) != GL_NO_ERROR) {

		}
		glGetTexImage(GL_TEXTURE_1D, 0, GL_RED, GL_INT, numberOfOccurrances);
		glFinish();
		
		while ((errorType = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << errorType << std::endl;
		}
		*/
		int rangeFit = 0;
		int k, j;
		std::cout << "X pos: " << intFocus.x << " Y pos: " << intFocus.y << " Z pos: " << intFocus.z << std::endl;
		for (i = -5; i < 6; i++) {
			for (k = -5; k < 6; k++) {
				for (j = -5; j < 6; j++) {
					rangeFit = 0;
					if (intFocus.z + i < 0 || intFocus.z + i > 224 || intFocus.y + k < 0 || intFocus.y + k > 255 || intFocus.x + j < 0 || intFocus.x + j > 255) {

					}
					else {
						int sampleValue = (int)volumeData[intFocus.x + i + 256 * (intFocus.y + k + 225 * intFocus.z + j)];
						if (sampleValue > 0) {
							rangeFit = sampleValue / upperBound;
							if (rangeFit == totalSubdivisions) {
								rangeFit--;
							}
							numberOfOccurrances[rangeFit]++;
						}
					}
				}
			}
		}
		int largestNumberOfOccurances = 0;
		for (i = 0; i < totalSubdivisions; i++) {
			if (numberOfOccurrances[i] > largestNumberOfOccurances) {
				largestNumberOfOccurances = numberOfOccurrances[i];
			}
		}
		float barWidth = 1.0f / (float)totalSubdivisions;
		glm::mat4 *modelMatrices;
		modelMatrices = (glm::mat4 *)malloc(totalSubdivisions * sizeof(glm::mat4));
		if (modelMatrices != NULL) {
			glm::mat4 identityMatrix = glm::mat4(1.0f);
			float barHeight = (float)numberOfOccurrances[0] / (float)largestNumberOfOccurances;
			glm::mat4 scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
			glm::mat4 translatePart = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			modelMatrices[0] = translatePart * scalePart * identityMatrix;
			for (i = 1; i < totalSubdivisions; i++) {
				barHeight = (float)numberOfOccurrances[i] / (float)largestNumberOfOccurances;
				scalePart = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
				translatePart = glm::translate(identityMatrix, glm::vec3((float)i * barWidth, 0.0f, 0.0f));
				modelMatrices[i] = translatePart * scalePart * identityMatrix;
			}
			glBindVertexArray(VAO);
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
			//free(offsets);
			free(modelMatrices);
			free(numberOfOccurrances);
			//glDeleteTextures(1, &histoTex);
		}

	}
}

void Histogram::DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix){
	glUseProgram(program);
	GLint orthoLocation;
	orthoLocation = glGetUniformLocation(program, "ProjectionMatrix");
	glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6 , totalSubdivisions);
	GLenum errorType;
	while ((errorType = glGetError()) != GL_NO_ERROR) {

	}
	glBindVertexArray(0);
}

void Histogram::CleanUp() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, VBO);
}

Histogram::~Histogram()
{
}

#include "..\Headers\Histogram.h"



Histogram::Histogram(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles){
	totalSubdivisions = numberOfSubdivisions;
	int *numberOfOccurrances;
	numberOfOccurrances = (int *) malloc(numberOfSubdivisions * sizeof(int));
	if (numberOfOccurrances != NULL) {

		int i;
		for (i = 0; i < numberOfSubdivisions; i++) {
			numberOfOccurrances[i] = 0;
		}
		int upperBound = 255 / numberOfSubdivisions;
		int totalSamples = xResolution * yResolution * numberOfFiles;
		int rangeFit = 0;
		for (i = 0; i < totalSamples; i++) {
			rangeFit = 0;
			int sampleValue = (int)volumeData[i];
			if (sampleValue > 0) {
				rangeFit = sampleValue / upperBound;
				if (rangeFit == numberOfSubdivisions) {
					rangeFit--;
				}
				numberOfOccurrances[rangeFit]++;
			}
		}
		int largestNumberOfOccurances = 0;
		for (i = 0; i < numberOfSubdivisions; i++) {
			if (numberOfOccurrances[i] > largestNumberOfOccurances) {
				largestNumberOfOccurances = numberOfOccurrances[i];
			}
		}
		float barWidth = 1.0f / (float)numberOfSubdivisions;
		vertices = (Vertex2d *)malloc(6 * sizeof(Vertex2d));
		if (vertices != NULL) {
			vertices[0].position[0] = -0.5f;
			vertices[0].position[1] = -0.5f;

			vertices[1].position[0] = 0.50f;
			vertices[1].position[1] = -0.5f;

			vertices[2].position[0] = 0.50f;
			vertices[2].position[1] = 0.50f;

			vertices[3].position[0] = 0.0f;
			vertices[3].position[1] = 0.0f;

			vertices[4].position[0] = 1.0f;
			vertices[4].position[1] = 1.0f;

			vertices[5].position[0] = 0.0f;
			vertices[5].position[1] = 1.0f;
			glm::mat4 *modelMatrices;
			modelMatrices = (glm::mat4 *)malloc(numberOfSubdivisions * sizeof(glm::mat4));
			if (modelMatrices != NULL) {
				glm::mat4 identityMatrix = glm::mat4(1.0f);
				for (i = 0; i < numberOfSubdivisions; i++) {
					float barHeight = (float)numberOfOccurrances[i] / (float)largestNumberOfOccurances;
					modelMatrices[i] = glm::scale(identityMatrix, glm::vec3(barWidth, barHeight, 0.0f));
					modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3((float)i * barWidth, 0.0f, 0.0f));
				}

				glGenBuffers(2, VBO);
				glGenVertexArrays(1, &VAO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
				glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);


				//glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
				//glBufferData(GL_ARRAY_BUFFER, numberOfSubdivisions * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid *)0);
				/*
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
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
				GLenum errorType;
				while ((errorType = glGetError()) != GL_NO_ERROR) {

				}

				*/
				glBindVertexArray(0);

				free(modelMatrices);
				free(vertices);
				free(numberOfOccurrances);
			}
		}
	}

}

void Histogram::DrawHistogram(GLuint program, glm::mat4 ProjectionMatrix){
	glUseProgram(program);
	GLint orthoLocation;
	orthoLocation = glGetUniformLocation(program, "ProjectionMatrix");
	glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);//, totalSubdivisions);
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

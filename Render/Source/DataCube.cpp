#include "..\Headers\DataCube.h"



DataCube::DataCube(){
	//Creates a unit cube centred at 
	vertices = (Vertex *)malloc((8) * sizeof(Vertex));
	texCoords = (Vertex *)malloc((8) * sizeof(Vertex));
	vertices[0].position[0] = -0.5f;
	vertices[0].position[1] = -0.5f;
	vertices[0].position[2] = -0.5f;

	vertices[1].position[0] = 0.5f;
	vertices[1].position[1] = -0.5f;
	vertices[1].position[2] = -0.5f;

	vertices[2].position[0] = 0.5f;
	vertices[2].position[1] = 0.5f;
	vertices[2].position[2] = -0.5f;

	vertices[3].position[0] = -0.5f;
	vertices[3].position[1] = 0.5f;
	vertices[3].position[2] = -0.5f;

	vertices[4].position[0] = -0.5f;
	vertices[4].position[1] = -0.5f;
	vertices[4].position[2] = 0.5f;

	vertices[5].position[0] = 0.5f;
	vertices[5].position[1] = -0.5f;
	vertices[5].position[2] = 0.5f;

	vertices[6].position[0] = 0.5f;
	vertices[6].position[1] = 0.5f;
	vertices[6].position[2] = 0.5f;

	vertices[7].position[0] = -0.5f;
	vertices[7].position[1] = 0.5f;
	vertices[7].position[2] = 0.5f;

	texCoords[0].position[0] = 0.0f;
	texCoords[0].position[1] = 0.0f;
	texCoords[0].position[2] = 0.0f;

	texCoords[1].position[0] = 1.0f;
	texCoords[1].position[1] = 0.0f;
	texCoords[1].position[2] = 0.0f;

	texCoords[2].position[0] = 1.0f;
	texCoords[2].position[1] = 1.0f;
	texCoords[2].position[2] = 0.0f;

	texCoords[3].position[0] = 0.0f;
	texCoords[3].position[1] = 1.0f;
	texCoords[3].position[2] = 0.0f;

	texCoords[4].position[0] = 0.0f;
	texCoords[4].position[1] = 0.0f;
	texCoords[4].position[2] = 1.0f;

	texCoords[5].position[0] = 1.0f;
	texCoords[5].position[1] = 0.0f;
	texCoords[5].position[2] = 1.0f;

	texCoords[6].position[0] = 1.0f;
	texCoords[6].position[1] = 1.0f;
	texCoords[6].position[2] = 1.0f;

	texCoords[7].position[0] = 0.0f;
	texCoords[7].position[1] = 1.0f;
	texCoords[7].position[2] = 1.0f;

	int *indices = (int *)malloc(6 * 6 * sizeof(int));
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;

	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;
	//
	indices[6] = 4;
	indices[7] = 0;
	indices[8] = 1;

	indices[9] = 4;
	indices[10] = 1;
	indices[11] = 5;
	//
	indices[12] = 1;
	indices[13] = 5;
	indices[14] = 2;

	indices[15] = 5;
	indices[16] = 6;
	indices[17] = 2;
	//
	indices[18] = 5;
	indices[19] = 4;
	indices[20] = 6;

	indices[21] = 4;
	indices[22] = 7;
	indices[23] = 6;
	//
	indices[24] = 4;
	indices[25] = 0;
	indices[26] = 7;

	indices[27] = 0;
	indices[28] = 3;
	indices[29] = 7;
	//
	indices[30] = 3;
	indices[31] = 2;
	indices[32] = 7;

	indices[33] = 2;
	indices[34] = 6;
	indices[35] = 7;

	//indices[6] = 
	ModelMatrix = glm::mat4(1.0f);
	glGenBuffers(3, VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), texCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);

	glBindVertexArray(0);
	free(indices);
	free(vertices);
	free(texCoords);
}

void DataCube::CleanUp() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(3, VBO);
}

void DataCube::DrawDataCube(GLuint program, GLuint volumeData) {
	GLint textureLocation;
	glUseProgram(program);
	glBindVertexArray(VAO);

	textureLocation = glGetUniformLocation(program, "TextureSampler");
	glUniform1i(textureLocation, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeData);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}

void DataCube::DrawDataCubeOrthoView(GLuint program, GLuint volumeData, GLuint gradientID, glm::mat4 OrthoMatrix, glm::mat4 ViewMatrix, glm::vec3 ViewingDir, glm::vec3 Resolution, glm::vec2 mousePosition, Light aLight, float *extents){
	GLint textureLocation, orthoLocation, viewLocation, viewingDirLocation, resolutionLocation, gradientLocation, lightLocation, modelLocation, extentLocation;
	GLint mousePosLocation;
	glUseProgram(program);
	glBindVertexArray(VAO);

	textureLocation = glGetUniformLocation(program, "TextureSampler");
	glUniform1i(textureLocation, 0);

	gradientLocation = glGetUniformLocation(program, "gradientTexture");
	glUniform1i(gradientLocation, 1);

	lightLocation = glGetUniformLocation(program, "myLight.intensity");
	glUniform3fv(lightLocation, 1, glm::value_ptr(aLight.intensity));

	lightLocation = glGetUniformLocation(program, "myLight.position");
	glUniform3fv(lightLocation, 1, glm::value_ptr(aLight.position));

	modelLocation = glGetUniformLocation(program, "ModelMatrix");
	orthoLocation = glGetUniformLocation(program, "OrthoMatrix");
	viewLocation = glGetUniformLocation(program, "ViewMatrix");
	viewingDirLocation = glGetUniformLocation(program, "ViewingDir");
	resolutionLocation = glGetUniformLocation(program, "Resolution");

	extentLocation = glGetUniformLocation(program, "xMinExtent");
	glUniform1f(extentLocation, extents[0]);

	extentLocation = glGetUniformLocation(program, "xMaxExtent");
	glUniform1f(extentLocation, extents[1]);

	extentLocation = glGetUniformLocation(program, "yMinExtent");
	glUniform1f(extentLocation, extents[2]);

	extentLocation = glGetUniformLocation(program, "yMaxExtent");
	glUniform1f(extentLocation, extents[3]);

	extentLocation = glGetUniformLocation(program, "zMinExtent");
	glUniform1f(extentLocation, extents[4]);

	extentLocation = glGetUniformLocation(program, "zMaxExtent");
	glUniform1f(extentLocation, extents[5]);

	mousePosLocation = glGetUniformLocation(program, "mousePosition");
	glUniform2fv(mousePosLocation, 1, glm::value_ptr(mousePosition));

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, glm::value_ptr(OrthoMatrix));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	glUniform3fv(viewingDirLocation, 1, glm::value_ptr(ViewingDir));
	glUniform3fv(resolutionLocation, 1, glm::value_ptr(Resolution));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeData);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, gradientID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void DataCube::MultiplyModelMatrix(glm::mat4 Transformation){
	ModelMatrix = Transformation * ModelMatrix;
}


void DataCube::ResetModelMatrix(){
	ModelMatrix = glm::mat4(1.0f);
}

DataCube::~DataCube()
{
}

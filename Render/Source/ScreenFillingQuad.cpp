#include "../Headers/ScreenFillingQuad.h"

ScreenFillingQuad::ScreenFillingQuad() {
	vertices = (Vertex *)malloc((4) * sizeof(Vertex));
	texCoords = (texCoord *)malloc((4) * sizeof(texCoord));
	vertices[0].position[0] = -1.0f;
	vertices[0].position[1] = -1.0f;
	vertices[0].position[2] = 0.0f;

	vertices[1].position[0] = 1.0f;
	vertices[1].position[1] = -1.0f;
	vertices[1].position[2] = 0.0f;

	vertices[2].position[0] = 1.0f;
	vertices[2].position[1] = 1.0f;
	vertices[2].position[2] = 0.0f;

	vertices[3].position[0] = -1.0f;
	vertices[3].position[1] = 1.0f;
	vertices[3].position[2] = 0.0f;

	texCoords[0].texCoordinate[0] = 0.0f;
	texCoords[0].texCoordinate[1] = 0.0f;

	texCoords[1].texCoordinate[0] = 1.0f;
	texCoords[1].texCoordinate[1] = 0.0f;

	texCoords[2].texCoordinate[0] = 1.0f;
	texCoords[2].texCoordinate[1] = 1.0f;

	texCoords[3].texCoordinate[0] = 0.0f;
	texCoords[3].texCoordinate[1] = 1.0f;

	int *indices;

	indices = (int *)malloc(6 * sizeof(int));
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 3;
	
	indices[3] = 0;
	indices[4] = 1;
	indices[5] = 2;

	glGenBuffers(3, VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), texCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid *)0);

	glBindVertexArray(0);
	free(indices);
	free(vertices);
	free(texCoords);

}

void ScreenFillingQuad::CleanUp() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(3, VBO);
}

void ScreenFillingQuad::DrawScreenFillingQuad(GLuint program, glm::mat4 FrustrumMatrix, glm::mat4 ViewMatrix, GLuint texture) {
	GLint ViewLocation, FrustrumLocation, textureLocation;

	glUseProgram(program);
	glBindVertexArray(VAO);

	ViewLocation = glGetUniformLocation(program, "View");
	FrustrumLocation = glGetUniformLocation(program, "Projection");

	glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(FrustrumLocation, 1, GL_FALSE, glm::value_ptr(FrustrumMatrix));

	textureLocation = glGetUniformLocation(program, "TextureSampler");
	glUniform1i(textureLocation, 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
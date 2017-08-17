#include "..\Headers\Gaussian.h"



Gaussian::Gaussian(){
	
}
Gaussian::Gaussian(int k) {
	int i;
	vertices = (Vertex *)malloc(200 * sizeof(Vertex));
	if (vertices != NULL) {
		for (i = 0; i < 200; i++) {
			vertices[i].position[0] = (float)i * (1.0f / 200.0f);
			vertices[i].position[1] = 1.0f;
			vertices[i].position[2] = 0.0f;
		}
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 200 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		a = 1.0f;
		b = 0.5f;
		c = 0.25f;
	}
}

Gaussian::~Gaussian()
{
}

void Gaussian::DrawGaussian(GLuint program, glm::mat4 ProjectionMatrix){
	glUseProgram(program);
	GLint orthoLocation;
	orthoLocation = glGetUniformLocation(program, "ProjectionMatrix");
	glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

	GLint floatLocations;
	floatLocations = glGetUniformLocation(program, "a");
	glUniform1f(floatLocations, a);

	floatLocations = glGetUniformLocation(program, "b");
	glUniform1f(floatLocations, b);

	floatLocations = glGetUniformLocation(program, "c");
	glUniform1f(floatLocations, c);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 200);
	glBindVertexArray(0);
}

#version 430 core
layout (location = 0) in vec2 position;
//layout (location = 1) in mat4 ModelMatrix;

uniform mat4 ProjectionMatrix;
out vec4 color;
void main(void){
	gl_Position = ProjectionMatrix * vec4(position.x, position.y, 0.0f, 1.0f);
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 ProjectionMatrix;

uniform float a;
uniform float b;
uniform float c;

void main(void){
	float numer = pow(position.x - b, 2.0f);
	float demon = 2.0 * pow(c, 2.0f);
	float hieght = a * exp(-(numer/demon));
	gl_Position = ProjectionMatrix * vec4(position.x, hieght, position.z, 1.0f);
}
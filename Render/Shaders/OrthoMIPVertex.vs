#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoords;

uniform mat4 View;
uniform mat4 Projection;

out vec3 textureCoords;

void main(){
	gl_Position = vec4(position, 1.0);
	textureCoords = texCoords;
}
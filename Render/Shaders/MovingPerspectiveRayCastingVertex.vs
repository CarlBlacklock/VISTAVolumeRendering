#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoords;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 textureCoords;
out vec3 worldPosition;

void main(){
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0f);
	textureCoords = texCoords;
	worldPosition = vec3(ViewMatrix * ModelMatrix * vec4(position, 1.0f));
}
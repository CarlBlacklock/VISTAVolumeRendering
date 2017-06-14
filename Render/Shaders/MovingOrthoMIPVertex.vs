#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoords;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 OrthoMatrix;
uniform vec3 Resolution;
uniform float xMinExtent;
uniform float xMaxExtent;
uniform float yMinExtent;
uniform float yMaxExtent;
uniform float zMinExtent;
uniform float zMaxExtent;
out vec3 textureCoords;


void main(){
	gl_Position = OrthoMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0f);
	textureCoords = texCoords;
	if(texCoords.x == 0.0f && xMinExtent != 0.0f){
		textureCoords.x = xMinExtent/Resolution.x; 
	}
	if(texCoords.x == 1.0f && xMaxExtent != Resolution.x){
		textureCoords.x = xMaxExtent/Resolution.x;
	}
	if(texCoords.y == 0.0f && yMinExtent != 0.0f){
		textureCoords.y = yMinExtent/Resolution.y; 
	}
	if(texCoords.y == 1.0f && yMaxExtent != Resolution.y){
		textureCoords.y = yMaxExtent/Resolution.y;
	}
	if(texCoords.z == 0.0f && zMinExtent != 0.0f){
		textureCoords.z = zMinExtent/Resolution.z; 
	}
	if(texCoords.z == 1.0f && zMaxExtent != Resolution.z){
		textureCoords.z = zMaxExtent/Resolution.z;
	}
}
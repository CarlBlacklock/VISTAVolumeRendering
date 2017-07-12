#version 430 core
layout (location = 0) in vec3 position;
//layout (location = 1) in float offset;
layout (location = 1) in mat4 ModelMatrix;

uniform mat4 ProjectionMatrix;


void main(void){
	if(position.x == 0.0f){
		gl_Position = ProjectionMatrix * ModelMatrix * vec4(position.x, position.yz, 1.0f);
	}
	else{
		gl_Position = ProjectionMatrix * ModelMatrix * vec4(position, 1.0f);
	}
	//color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
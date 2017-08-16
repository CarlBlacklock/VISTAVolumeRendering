#version 430 core
layout (location = 0) in vec3 position;
//layout (location = 1) in float offset;
layout (location = 1) in mat4 ModelMatrix;

uniform sampler1D counts;
uniform mat4 ProjectionMatrix;

//out vec4 color;
void main(void){
	float countValue = texelFetch(counts, gl_InstanceID, 0).r;
	vec4 testPos = ModelMatrix * vec4(position, 1.0f);
	gl_Position = ProjectionMatrix * ModelMatrix * vec4(position.x, position.y * countValue, position.z, 1.0f);
	//color = vec4(0.0f, countValue, 0.0f, 1.0f);
}
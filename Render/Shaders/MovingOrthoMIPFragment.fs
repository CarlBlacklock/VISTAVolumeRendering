#version 430 core

in vec3 textureCoords;

uniform sampler3D TextureSampler;
uniform vec3 ViewingDir;
uniform vec3 Resolution;
uniform float xMinExtent;
uniform float xMaxExtent;
uniform float yMinExtent;
uniform float yMaxExtent;
uniform float zMinExtent;
uniform float zMaxExtent;

out vec4 color;

void main(void){
	int depth = 256;
	vec3 stepSizes = 1.0f / Resolution;
	vec3 viewDir = normalize(ViewingDir);
	vec3 step = vec3(viewDir.x * stepSizes.x, viewDir.y * stepSizes.y, viewDir.z * stepSizes.z);
	float xMin = xMinExtent/Resolution.x;
	float xMax = xMaxExtent/Resolution.x;
	float yMin = yMinExtent/Resolution.y;
	float yMax = yMaxExtent/Resolution.y;
	float zMin = zMinExtent/Resolution.z;
	float zMax = zMaxExtent/Resolution.z;
	vec4 currentColor;
	vec3 currentPos = textureCoords;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i = 0; i < depth; i++){
		float sampleValue = texture(TextureSampler, currentPos).r;
		if(sampleValue >= 0.1){
			currentColor = vec4(sampleValue, sampleValue, sampleValue, sampleValue);
			color = max(color, currentColor);
		}
		if(color.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
			break;
		}
		currentPos += step;
	}

}
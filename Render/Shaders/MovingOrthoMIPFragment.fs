#version 430 core

in vec3 textureCoords;

subroutine void filterFunction();
subroutine uniform filterFunction FilterChoice;

uniform sampler3D TextureSampler;
uniform vec3 ViewingDir;
uniform vec3 Resolution;
uniform float xMinExtent;
uniform float xMaxExtent;
uniform float yMinExtent;
uniform float yMaxExtent;
uniform float zMinExtent;
uniform float zMaxExtent;
uniform float alpha;
uniform float beta;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;


subroutine (filterFunction) void noFilter(){
	//Choosen when there is no desire to filter the input image
	vec3 stepSizes = 1.0f / Resolution;
	vec3 viewDir = normalize(ViewingDir);
	vec3 step = vec3(viewDir.x * stepSizes.x, viewDir.y * stepSizes.y, viewDir.z * stepSizes.z);
	float xMin = xMinExtent/Resolution.x;
	float xMax = xMaxExtent/Resolution.x;
	float yMin = yMinExtent/Resolution.y;
	float yMax = yMaxExtent/Resolution.y;
	float zMin = zMinExtent/Resolution.z;
	float zMax = zMaxExtent/Resolution.z;
	vec3 currentPos = textureCoords;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	while(!(currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax)){
		float sampleValue = texture(TextureSampler, currentPos).r;
		if(sampleValue >= 0.1){
			if(sampleValue > color.r){
				color = vec4(sampleValue, sampleValue, sampleValue, sampleValue);
				position = currentPos;
			}
		}
		
		//if(color.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
			//break;
		//}
		currentPos += step;
	}
	
}

subroutine (filterFunction) void sobelGaussFilter(){
	vec3 stepSizes = 1.0f / Resolution;
	vec3 viewDir = normalize(ViewingDir);
	vec3 step = vec3(viewDir.x * stepSizes.x, viewDir.y * stepSizes.y, viewDir.z * stepSizes.z);
	float xMin = xMinExtent/Resolution.x;
	float xMax = xMaxExtent/Resolution.x;
	float yMin = yMinExtent/Resolution.y;
	float yMax = yMaxExtent/Resolution.y;
	float zMin = zMinExtent/Resolution.z;
	float zMax = zMaxExtent/Resolution.z;
	vec3 currentPos = textureCoords;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	while(!(currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax)){
		vec2 filterValues = texture(TextureSampler, currentPos).rg;
		//Solve the filter equation: alpha * f(x,y,z) + beta * g(x,y,z)
		float sampleValue = alpha * filterValues.x + beta * filterValues.y;
		if(sampleValue >= 0.1){
			if(sampleValue > color.r){
				color = vec4(sampleValue, sampleValue, sampleValue, sampleValue);
				position = currentPos;
			}
		}
		
		//if(color.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
			//break;
		//}
		currentPos += step;
	}
	
}

void main(void){
	FilterChoice();
}
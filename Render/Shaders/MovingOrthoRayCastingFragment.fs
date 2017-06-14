#version 430 core

in vec3 textureCoords;
struct Light{
	vec3 position;
	vec3 intensity;
};
uniform sampler3D TextureSampler;
uniform sampler3D gradientTexture;
uniform Light myLight;
uniform vec3 ViewingDir;
uniform vec3 Resolution;
uniform float xMinExtent;
uniform float xMaxExtent;
uniform float yMinExtent;
uniform float yMaxExtent;
uniform float zMinExtent;
uniform float zMaxExtent;


vec3 diffuse(vec3 position, vec3 currentColor){
	vec3 gradient = texture(gradientTexture, position).rgb;
	float gradientIntensity = sqrt( gradient.x*gradient.x + gradient.y*gradient.y + gradient.z*gradient.z);
	//if(gradientIntensity > 0.1){
		vec3 normal = normalize(gradient);
		vec3 lightDir = vec3(normalize(myLight.position - position));
		float LdotN = max(dot(normal, lightDir), 0.0);
		return vec3(LdotN + currentColor.r, LdotN + currentColor.g, LdotN + currentColor.g);
	//}
	//else{
		//return vec3(0.0f, 0.0f, 0.0f);
	//}
}

out vec4 color;

void main(void){
	int depth = 256;
	vec3 stepSizes = 1.0f / Resolution;
	vec3 viewDir = normalize(ViewingDir);
	vec3 step = vec3(viewDir.x * stepSizes.x, viewDir.y * stepSizes.y, viewDir.z * stepSizes.z);
	//vec3 step = viewDir * 1.732f * stepSize;
	vec4 currentColor;
	vec3 currentPos = vec3(textureCoords.x, textureCoords.y, textureCoords.z);
	vec3 gradient;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	float gradientMagnitude;
	float xMin = xMinExtent/Resolution.x;
	float xMax = xMaxExtent/Resolution.x;
	float yMin = yMinExtent/Resolution.y;
	float yMax = yMaxExtent/Resolution.y;
	float zMin = zMinExtent/Resolution.z;
	float zMax = zMaxExtent/Resolution.z;
	//float stepCount = 0.0f;
	while(true){
		float sampleValue = texture(TextureSampler, currentPos).r;
		//Attempt to use the gradient magnitude as a method of detecting contours
		gradient = vec3(texture(gradientTexture, currentPos).rgb);
		//Calculate gradient magnitude
		gradientMagnitude = length(gradient);
		//if(gradientMagnitude > 419.0f){
			currentColor = vec4(sampleValue,sampleValue,sampleValue, sampleValue);
			color.rgb = color.rgb + (1 - color.a) * currentColor.a * currentColor.rgb;
			color.a   = color.a   + (1 - color.a) * currentColor.a;
		//}
		currentPos += step;
		if(color.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
			break;
		}
		
	}
	//color = vec4(viewDir, 1.0f);
}
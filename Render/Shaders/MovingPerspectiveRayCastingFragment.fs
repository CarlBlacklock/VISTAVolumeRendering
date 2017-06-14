#version 430 core

in vec3 textureCoords;
in vec3 worldPosition;
struct Light{
	vec3 position;
	vec3 intensity;
};
uniform sampler3D TextureSampler;
uniform sampler3D gradientTexture;
uniform Light myLight;
uniform vec3 ViewingDir;
uniform vec3 Resolution;
uniform vec3 cameraPosition;

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
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	vec3 step = vec3(viewDir.x * stepSizes.x, viewDir.y * stepSizes.y, viewDir.z * stepSizes.z);
	//vec3 step = viewDir * 1.732f * stepSize;
	vec4 currentColor;
	vec3 currentPos = vec3(textureCoords.x, 1.0f - textureCoords.y, textureCoords.z);
	vec3 gradient;
	vec3 diffuseColor;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	//float stepCount = 0.0f;
	while(true){
		float sampleValue = texture(TextureSampler, currentPos).r;
		//if(sampleValue >= 0.15f){
			currentColor = vec4(sampleValue,sampleValue,sampleValue, sampleValue);
			gradient = vec3(texture(gradientTexture, currentPos).rgb);
			//diffuseColor = diffuse(currentPos, currentColor.rgb);
			//currentColor = vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, sampleValue);
			color.rgb = color.rgb + (1 - color.a) * currentColor.a * currentColor.rgb;
			color.a   = color.a   + (1 - color.a) * currentColor.a;
		//}
		currentPos += step;
		//stepCount += 1.0f;
		if(color.a == 1.0 || currentPos.x < 0.0f || currentPos.y < 0.0f || currentPos.z < 0.0f || currentPos.x > 1.0f || currentPos.y > 1.0f || currentPos.z > 1.0f){
			break;
		}
		
	}
	//color = vec4(viewDir, 1.0f);
}
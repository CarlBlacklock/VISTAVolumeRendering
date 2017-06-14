#version 430 core

in vec3 textureCoords;

uniform sampler3D TextureSampler;

out vec4 color;

void main(void){
	int depth = 256;
	float stepSize = 1.0/256.0;
	vec3 step = vec3(0.0, 0.0, stepSize);
	vec4 currentColor;
	vec3 currentPos = textureCoords;
	color = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i = 0; i < depth; i++){
		float sampleValue = texture(TextureSampler, currentPos).r;
		currentColor = vec4(sampleValue, sampleValue, sampleValue, sampleValue);
		color.rgb = color.rgb + (1 - color.a) * currentColor.a * currentColor.rgb;
		color.a   = color.a   + (1 - color.a) * currentColor.a;
		if(color.a == 1.0){
			break;
		}
		currentPos += step;
	}
}
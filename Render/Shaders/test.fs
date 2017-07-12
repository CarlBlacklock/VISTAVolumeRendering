#version 430 core
in vec2 TexCoords;

uniform sampler2D TextureSampler;
out vec4 color;
void main(){
	color  = texture(TextureSampler, TexCoords);
}
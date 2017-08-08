#version 430 core

in vec3 textureCoords;
layout (origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
struct Light{
	vec3 position;
	vec3 intensity;
};

subroutine void filterFunction();
subroutine uniform filterFunction FilterChoice;

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
uniform vec2 mousePosition;
uniform float alpha;
uniform float beta;

out vec4 color;

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

vec3 getSamplePos(vec3 position, int sampleNumber){
	float xMin = xMinExtent/Resolution.x;
	float xMax = xMaxExtent/Resolution.x;
	float yMin = yMinExtent/Resolution.y;
	float yMax = yMaxExtent/Resolution.y;
	float zMin = zMinExtent/Resolution.z;
	float zMax = zMaxExtent/Resolution.z;
	vec3 samplePos;
	if(position.x == xMin || position.x == xMax){
		
	}
	return position;
}

subroutine (filterFunction) void noFilter(){
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
	float mouseDistance = distance(gl_FragCoord.xy, mousePosition);
	int numberOfSamplesToTake;
	if(mouseDistance <= 100.0f){
		if(mouseDistance <= 20.0f){
			numberOfSamplesToTake = 4;
		}
		else if(mouseDistance <= 60.0f){
			numberOfSamplesToTake = 3;
		}
		else{
			numberOfSamplesToTake = 2;
		}
		vec4 fragmentSampleColor;
		bool keepCollectingSamples = true;
		bool keepWorkingOnCurrentSample;
		int sampleNumber = 0;
		vec3 samplePos;
		while(keepCollectingSamples){
			keepWorkingOnCurrentSample = true;
			fragmentSampleColor = vec4(0.0, 0.0, 0.0, 0.0);
			currentPos = vec3(textureCoords.x, textureCoords.y, textureCoords.z);

			while(keepWorkingOnCurrentSample){
				float sampleValue = texture(TextureSampler, currentPos).r;
				//Attempt to use the gradient magnitude as a method of detecting contours
				//gradient = vec3(texture(gradientTexture, currentPos).rgb);
				//Calculate gradient magnitude
				//gradientMagnitude = length(gradient);
				//if(gradientMagnitude > 419.0f){
					currentColor = vec4(sampleValue,sampleValue,sampleValue, sampleValue);
					fragmentSampleColor.rgb = fragmentSampleColor.rgb + (1 - fragmentSampleColor.a) * currentColor.a * currentColor.rgb;
					fragmentSampleColor.a   = fragmentSampleColor.a   + (1 - fragmentSampleColor.a) * currentColor.a;
				//}
				currentPos += step;
				if(fragmentSampleColor.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
					color += fragmentSampleColor;
					keepWorkingOnCurrentSample = false;
				}
			}
			sampleNumber++;
			if(sampleNumber == numberOfSamplesToTake){
				keepCollectingSamples = false;
			}
		}
		color = color / float(numberOfSamplesToTake);
	}

	else{
		bool continueRendering = true;
		while(continueRendering){
			float sampleValue = texture(TextureSampler, currentPos).r;
			//Attempt to use the gradient magnitude as a method of detecting contours
			//gradient = vec3(texture(gradientTexture, currentPos).rgb);
			//Calculate gradient magnitude
			//gradientMagnitude = length(gradient);
			//if(gradientMagnitude > 419.0f){
				currentColor = vec4(sampleValue,sampleValue,sampleValue, sampleValue);
				color.rgb = color.rgb + (1 - color.a) * currentColor.a * currentColor.rgb;
				color.a   = color.a   + (1 - color.a) * currentColor.a;
			//}
			currentPos += step;
			if(color.a == 1.0 || currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax){
				continueRendering = false;
			}
		
		}
	}
	//color = vec4(viewDir, 1.0f);
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
	vec4 currentColor = vec4(0.0, 0.0, 0.0, 0.0);
	color = vec4(0.0, 0.0, 0.0, 0.0);
	while(!(currentPos.x < xMin || currentPos.y < yMin || currentPos.z < zMin || currentPos.x > xMax || currentPos.y > yMax || currentPos.z > zMax || color.a >= 1.0f)){
		vec2 filterValues = texture(TextureSampler, currentPos).rg;
		//Solve the filter equation: alpha * f(x,y,z) + beta * g(x,y,z)
		float sampleValue = alpha * filterValues.x + beta * filterValues.y;
		if(sampleValue >= 0.1){
			currentColor = vec4(sampleValue,sampleValue,sampleValue, sampleValue);
			color.rgb = color.rgb + (1 - color.a) * currentColor.a * currentColor.rgb;
			color.a   = color.a   + (1 - color.a) * currentColor.a;
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
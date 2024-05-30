#version 430 core

layout(binding = 0) uniform sampler2D u_diffuse;
layout(binding = 1) uniform sampler2D u_occlusion;

uniform int u_blurStrength = 3;

in vec2 texCoords;

out vec4 out_fragColor;

void main() {
	vec2 pixelSize = 1 / vec2(textureSize(u_occlusion, 0));
	float result;
	
	if (u_blurStrength == 0) {
	out_fragColor = vec4(texture(u_diffuse, texCoords).xyz ,1);
	return;
	}
	
	int lowerRange = -u_blurStrength / 2;
	int higherRange = -lowerRange + (u_blurStrength % 2 == 0 ? 0 : 1);
	
	
	for (int x = lowerRange; x < higherRange; ++x){
		for (int y = lowerRange; y < higherRange; ++y)
		{
			vec2 offset = vec2(x,y) * pixelSize;
			result += texture(u_occlusion, texCoords + offset).r;
		}
	}
	result /= (u_blurStrength * u_blurStrength);
	
	//out_fragColor = vec4(vec3(result),1);
	out_fragColor = vec4(texture(u_diffuse, texCoords).xyz * result,1);
}
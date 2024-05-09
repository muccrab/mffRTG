#version 430 core

layout(binding = 0) uniform sampler2D u_diffuseTexture;
layout(binding = 1) uniform sampler2D u_shadowMap;
/* layout(binding = 1) uniform sampler2D u_specularTexture; */
/* layout(binding = 2) uniform sampler2D u_normalTexture; */
/* layout(binding = 3) uniform sampler2D u_displacementTexture; */
/* layout(binding = 4) uniform sampler2D u_ambientOccTexture; */

layout(location = 15) uniform vec3 u_lightPos;
layout(location = 20) uniform mat4 u_lightMat;
layout(location = 40) uniform mat4 u_lightProjMat;

in vec4 position;
in vec2 texCoords;
in vec3 normal;

out vec4 out_color;
out vec3 out_normal;
out vec3 out_position;
out vec3 out_shadow;


void main() {
	out_color = texture(u_diffuseTexture, texCoords);
	out_normal = normalize(normal);
	out_position = position.xyz/position.w;
	
	vec4 shadowCoords = (u_lightProjMat * u_lightMat * position);
	vec3 mappedShadowCoords = (shadowCoords.xyz/shadowCoords.w) * 0.5 + 0.5;
	if (mappedShadowCoords.x > 0 && mappedShadowCoords.x < 1
		&& mappedShadowCoords.y > 0 && mappedShadowCoords.y < 1) {
		float shadow = texture(u_shadowMap, mappedShadowCoords.xy).x;
		if (shadow < (mappedShadowCoords.z - 0.0001)) {
			out_shadow = vec3(0.0);
		} else {
			out_shadow = vec3(1.0);
		}
	}
	
}


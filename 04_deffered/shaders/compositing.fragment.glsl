#version 430 core

layout(binding = 0) uniform sampler2D u_diffuse;
layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_position;
layout(binding = 5) uniform sampler2D u_shadowMap;

uniform vec3 u_lightPos;

const vec3 ambientColor = vec3(0.1, 0.1, 0.1);

in vec2 texCoords;

out vec4 fragColor;
void main() {
	vec3 position = texture(u_position, texCoords).xyz;
	vec3 normal = texture(u_normal, texCoords).xyz;
	vec3 diffuseColor = texture(u_diffuse, texCoords).xyz;
	float shadow = texture(u_shadowMap, texCoords).x;

	fragColor = vec4(diffuseColor, 1.0);
	//fragColor = vec4(position, 1.0);
	//fragColor = vec4(normal, 1.0);
	//fragColor = vec4(shadow, shadow, shadow, 1.0);
}


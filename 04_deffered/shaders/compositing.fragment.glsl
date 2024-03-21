#version 430 core

layout(binding = 0) uniform sampler2D u_diffuse;
layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_shadow;
layout(binding = 3) uniform sampler2D u_position;

in vec2 texCoords;

out vec4 diffuseColor;
void main() {
	diffuseColor = texture(u_diffuse, texCoords);
	// diffuseColor = vec4(1.0, diffuseColor.y, 0.0, 1.0);
	// // diffuseColor = vec4(texCoords, 0.0, 1.0);
}


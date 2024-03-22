#version 430 core

/* uniform vec3 u_viewPos; */

out vec4 out_fragColor;

/* in vec3 position; */

void main() {
	float near = 0.01; // Camera's near plane
	float far = 100.0;  // Camera's far plane
	
	float z = gl_FragCoord.z; // Non-linear depth value
	float linearDepth = (2.0 * near) / (far + near - z * (far - near));
	out_fragColor = vec4(linearDepth, 0.0, 0.0, 1.0);
}


#version 430 core
layout(lines) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 fragPos[];
in float fragThickness[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 direction = normalize(fragPos[1] - fragPos[0]);
	vec3 up = vec3(0.0, 1.0, 0.0);
	if (abs(direction.y) > 0.99)
		up = vec3(1.0, 0.0, 0.0);
	vec3 right = normalize(cross(direction, up));
	vec3 forward = normalize(cross(right, direction));

	vec3 startLeft = fragPos[0] - right * fragThickness[0] * 0.5;
	vec3 startRight = fragPos[0] + right * fragThickness[0] * 0.5;
	vec3 endLeft = fragPos[1] - right * fragThickness[1] * 0.5;
	vec3 endRight = fragPos[1] + right * fragThickness[1] * 0.5;

	vec3 startUp = fragPos[0] + forward * fragThickness[0] * 0.5;
	vec3 startDown = fragPos[0] - forward * fragThickness[0] * 0.5;
	vec3 endUp = fragPos[1] + forward * fragThickness[1] * 0.5;
	vec3 endDown = fragPos[1] - forward * fragThickness[1] * 0.5;

	// Emit vertices for the 4-sided tube
	// Start
	gl_Position = projection * view * model * vec4(startLeft, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(startRight, 1.0);
	EmitVertex();

	// End
	gl_Position = projection * view * model * vec4(endLeft, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(endRight, 1.0);
	EmitVertex();

	EndPrimitive();

	// Top
	gl_Position = projection * view * model * vec4(startUp, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(startDown, 1.0);
	EmitVertex();

	gl_Position = projection * view * model * vec4(endUp, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(endDown, 1.0);
	EmitVertex();

	EndPrimitive();

	// Side 1
	gl_Position = projection * view * model * vec4(startLeft, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(startUp, 1.0);
	EmitVertex();

	gl_Position = projection * view * model * vec4(endLeft, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(endUp, 1.0);
	EmitVertex();

	EndPrimitive();

	// Side 2
	gl_Position = projection * view * model * vec4(startRight, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(startDown, 1.0);
	EmitVertex();

	gl_Position = projection * view * model * vec4(endRight, 1.0);
	EmitVertex();
	gl_Position = projection * view * model * vec4(endDown, 1.0);
	EmitVertex();

	EndPrimitive();
}


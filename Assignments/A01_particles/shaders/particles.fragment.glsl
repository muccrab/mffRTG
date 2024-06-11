#version 430 core

uniform sampler3D u_particleTexture;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec2 u_disapearBound = vec2(0.2, 0.8); 


in float f_particleIndex;
in vec4 f_fragColor;
in vec2 f_texCoord;
in float f_particleZ;
in vec3 f_normal;
in vec3 f_worldPos;
in float f_lifeTime;

out vec4 out_fragColor;

void main() {
	//vec3 loc_viewDir = normalize(u_viewPos - f_worldPos);
	
	// Get Light Direction
	vec3 loc_lightDir = normalize(u_lightPos - f_worldPos);
	
	// Get Color from a 3d texture
	vec4 loc_color = texture(u_particleTexture, vec3(f_texCoord, f_particleZ));
	//out_fragColor = vec4(f_fragColor.xyz, loc_color.r);
	//float loc_scatter = dot(loc_viewDir, loc_lightDir);
	
	// Calculate Diffuse Part
	float loc_diffuse = dot(f_normal,loc_lightDir);
	//out_fragColor = vec4(f_fragColor.xyz * (0.5 * loc_scatter + 0.5),f_fragColor.a * loc_color.r) ;
	//out_fragColor = vec4(f_normal, 1);
	
	// Get Alpha multiplier based on life time
	float loc_alpha;
	if (f_lifeTime < u_disapearBound.x) loc_alpha = f_lifeTime / u_disapearBound.x;
	else if (f_lifeTime > u_disapearBound.y) loc_alpha = (1 - f_lifeTime) / (1 - u_disapearBound.y);
	else loc_alpha = 1;
	
	// Calculate final color of fragment
	out_fragColor = vec4(f_fragColor.xyz * (loc_diffuse * 0.6 + 0.4), loc_color.r * loc_alpha);
	//out_fragColor = vec4(f_normal, 1);
}

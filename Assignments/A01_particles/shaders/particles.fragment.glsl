#version 430 core

uniform sampler3D u_particleTexture;

in float f_particleIndex;
in vec4 f_fragColor;
in vec2 f_texCoord;
in float f_particleZ;
in vec4 f_normal;

out vec4 out_fragColor;

void main() {
	
	vec4 loc_lightDir = normalize(vec4(1,1,1,0));
	vec4 loc_color = texture(u_particleTexture, vec3(f_texCoord, f_particleZ));
	//out_fragColor = vec4(f_fragColor.xyz, loc_color.r);
	out_fragColor = vec4(f_fragColor.xyz * (dot(loc_lightDir , f_normal) + 0.1),f_fragColor.a * loc_color.r) ;
	
}

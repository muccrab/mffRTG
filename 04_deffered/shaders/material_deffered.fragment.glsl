#version 430 core

layout(binding = 0) uniform sampler2D u_diffuseTexture;
layout(binding = 1) uniform sampler2D u_specularTexture;
layout(binding = 2) uniform sampler2D u_normalTexture;
layout(binding = 3) uniform sampler2D u_displacementTexture;
layout(binding = 4) uniform sampler2D u_ambientOccTexture;
layout(binding = 6) uniform sampler2D u_shadowMap;
//layout(binding = 6) uniform sampler2DShadow u_shadowMap;

in vec4 position;
in vec2 texCoords;
in vec3 normal;
in vec4 shadowCoords;

out vec4 out_color;
out vec3 out_normal;
out vec3 out_position;
// out float out_shadow;


void main() {
	// vec3 diffuseColor = texture(u_diffuseTexture, texCoords).rgb;
	// vec3 specColor = texture(u_specularTexture, texCoords).rgb;
        //
        //
	// vec3 sc = (shadowCoords.xyz)/shadowCoords.w * 0.5 + 0.5;
        //
	// if (sc.x < 0 || sc.x > 1 || sc.y < 0 || sc.y > 1) {
	// 	out_shadow = 0.5;//texture(u_diffuseTexture, texCoords).g;
	// 	//diffuseColor = 0.5 * diffuseColor;
	// } else {
	// 	out_shadow = 1.0;
	// 	/*for (int i=0;i<4;i++){
	// 		if ((sc.z -0.00001)> texture(u_shadowMap, sc.xy + (poissonDisk[i]/700.0)).x) {
	// 			out_shadow -= 0.15;
	// 		}
	// 	}*/
	// 	if ((sc.z - u_bias)> texture(u_shadowMap, sc.xy).x) {
	// 		out_shadow = 0.5;
	// 	}
	// 	//out_shadow = 0.5 + 0.5*texture(u_shadowMap, sc, -0.001);
	// }
        //
	//diffuseColor = out_shadow*diffuseColor;
	// out_color = vec4(diffuseColor, 1.0);
	// out_normal = normalize(normal);
	// out_position = position.xyz/position.w;
	out_color = texture(u_diffuseTexture, texCoords);
	out_normal = normalize(normal);
	out_position = position.xyz/position.w;
}


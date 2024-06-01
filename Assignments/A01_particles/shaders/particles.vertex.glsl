#version 430 core

#define PI 3.1415926535

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

uniform int u_instanceCount;
uniform uint u_time;

uniform vec3 u_emmiterDimensions = vec3(1,1,1) * 0.1;
uniform vec2 u_particleLifeSize = vec2(0.05,.5);
uniform float u_particleSpeed = 0.1;
uniform vec3 u_particleDirection = vec3(0,1,0);
uniform float u_particleSpread = 45;
uniform vec2 u_particleLifeVelocity = vec2(1,3);
uniform float u_rotationSpeed = 0;
uniform float u_initRotation = 45;
uniform vec2 u_gravityLifeForce = vec2(0,0.1);
uniform vec2 u_windLifeSpeed = vec2(0.5, 1);
uniform vec3 u_windDirection = vec3(1,0,1);
uniform vec4 u_startColor = vec4(0.3, 0.3, 0.3, 1);
uniform vec4 u_endColor = vec4(1, 1, 1, 0);



layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_texCoord;


out float f_particleIndex;
out vec4 f_fragColor;
out vec2 f_texCoord;
out float f_particleZ;
out vec4 f_normal;

// From Ben Cloward Youtube's series on Particle System in Unity/Unreal
vec3 hash33(uint x, uint y, uint z){
        x ^= 1103515245U;
        y ^= x + z;
        y = y * 134775813;
        z += x ^ y;
        y += x ^ z;
        x += y * z;
        x = x * 0x27d4eb2du;
        z ^= x << 3;
        y += z << 3;
		float m = (1 / float(0x7fffffff));
        vec3 o = vec3(x, y,z);
        return o * m;
    }
    


mat4 getModelView(float a_scale){
	mat4 loc_modifiedModel = u_modelMat;
	//	// Column 0:
	loc_modifiedModel[0][0] = a_scale;
	loc_modifiedModel[0][1] = 0;
	loc_modifiedModel[0][2] = 0;

	// Column 1:
	loc_modifiedModel[1][0] = 0;
	loc_modifiedModel[1][1] = a_scale;
	loc_modifiedModel[1][2] = 0;
	
	// Column 2:
	loc_modifiedModel[2][0] = 0;
	loc_modifiedModel[2][1] = 0;
	loc_modifiedModel[2][2] = a_scale;
	
	return u_viewMat * loc_modifiedModel;
}


mat4 getRotationMatrix(float rotation){
	mat4 rotZ = mat4(0);
	rotZ[0][0] = cos(rotation);
	rotZ[0][1] = sin(rotation);
	rotZ[1][0] = -sin(rotation);
	rotZ[1][1] = cos(rotation);
	rotZ[2][2] = 1;
	rotZ[3][3] = 1;
	return rotZ;
}


vec4 getBillboardPosition(vec3 a_positionOffset, float a_scale, float rotation){
	
	mat4 loc_rotMat = getRotationMatrix(rotation);
	mat4 loc_modelView = getModelView(a_scale);
	vec3 loc_vert = (inverse(u_viewMat) * loc_rotMat * vec4(in_vert, 1)).xyz; // inverse of view makes it so they are rotated to the camera. f**king pogchamp.
	
	vec4 loc_position = vec4(loc_vert + a_positionOffset, 1);
	return loc_modelView * loc_position;
}



void main(void)
{
	float loc_particleIndex = clamp(gl_InstanceID / float(u_instanceCount), 0, 1);
	f_particleIndex = loc_particleIndex;

	float loc_tempLifeTime = (float(u_time)/1000) * u_particleSpeed + loc_particleIndex;
	float loc_lifeTime = (loc_tempLifeTime - floor(loc_tempLifeTime)) ;
	
	float loc_scale = mix(u_particleLifeSize.x, u_particleLifeSize.y, loc_lifeTime);
	float loc_velocity = mix(u_particleLifeVelocity.x, u_particleLifeVelocity.y, loc_lifeTime);
	float loc_gravity = mix(u_gravityLifeForce.x, u_gravityLifeForce.y, loc_lifeTime);
	float loc_wind = mix(u_windLifeSpeed.x, u_windLifeSpeed.y, loc_lifeTime);
	
	
	vec3 loc_hashedIndex = hash33(uint(loc_particleIndex * 255 + loc_tempLifeTime),
	uint(loc_particleIndex * 255 + loc_tempLifeTime),
	uint(loc_particleIndex * 255 + loc_tempLifeTime));

	
	
	float loc_particleSpread = u_particleSpread / 180;
	vec3 loc_hashedPosition = loc_hashedIndex * 2 - vec3(1);
	
	vec3 loc_spreadPosition = loc_hashedPosition * loc_particleSpread + u_particleDirection;
	loc_spreadPosition /= length(loc_spreadPosition);
	loc_spreadPosition *= loc_velocity * (loc_lifeTime);
	
	vec3 loc_forcePosition = vec3(0,-1,0) * loc_gravity + u_windDirection * loc_wind;
	
	
	vec3 loc_positionOffset = loc_hashedPosition * u_emmiterDimensions;
	
	float loc_rot = u_initRotation * PI / 180;
	loc_rot += (loc_lifeTime+ loc_hashedIndex.x) * u_rotationSpeed * 2 * PI;
	
	vec4 loc_position = getBillboardPosition(loc_positionOffset + loc_spreadPosition + loc_forcePosition, 
											loc_scale,
											loc_rot);
											
	gl_Position = u_projMat * loc_position;
	f_texCoord = in_texCoord;
	float loc_tempZ = loc_tempLifeTime - 2*(int(loc_tempLifeTime) / 2); 
	f_particleZ = loc_tempZ > 1 ? 2 - loc_tempZ : loc_tempZ;
	f_fragColor = mix(u_startColor, u_endColor, loc_lifeTime);
	
	f_normal = normalize(inverse(u_viewMat) * vec4(0,0,1,1)); // If this gonna work, I will be passing this normal as a uniform, I will save on performance.
	
}

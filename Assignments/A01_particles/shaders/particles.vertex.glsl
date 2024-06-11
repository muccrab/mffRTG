#version 430 core

#define PI 3.1415926535

uniform mat4 u_modelMat; // Model Matrix
uniform mat4 u_viewMat; // View Matrix
uniform mat4 u_projMat; // Projection Matrix

uniform int u_instanceCount; // Number of instances
uniform uint u_time; // Current time

uniform vec3 u_emmiterDimensions = vec3(1,1,1) * 0.1; // Dimensions of emmiter box
uniform vec2 u_particleLifeSize = vec2(0.05,.5); // Size of the particle durring lifetime
uniform float u_particleSpeed = 0.1; // Speed of particle life time
uniform vec3 u_particleDirection = vec3(0,1,0); // Direction where the particle is going
uniform float u_particleSpread = 45; // Spread angle of particle direction
uniform vec2 u_particleLifeVelocity = vec2(1,3); // The Speed/Velocity of particle during life time
uniform float u_rotationSpeed = 0; // The Rotation Speed/Velocity of particle during life time
uniform float u_initRotation = 45; // The initial rotation of particle
uniform vec2 u_gravityLifeForce = vec2(0,0.1); // Gracity Force during particle life time
uniform vec2 u_windLifeSpeed = vec2(0.5, 1); // Wind Speed during particle life time
uniform vec3 u_windDirection = vec3(1,0,1); // The Direction of the wind during particle life time
uniform vec4 u_startColor = vec4(0.3, 0.3, 0.3, 1); // The Start Color of the particle
uniform vec4 u_endColor = vec4(1, 1, 1, 0); // The End Color of the particle
uniform float u_normalSpread = 3; // It is used to controll normal vector on billboards. 0 means normal stays the same, while positive number means normal differs on the edges, Higher the number the more spreaded it is.


layout(location = 0) in vec3 in_vert; 
layout(location = 1) in vec2 in_texCoord;


out float f_particleIndex; // The identification of given particle
out vec4 f_fragColor; // Color of the particle
out vec2 f_texCoord; // Texture coordinates for particle
out float f_particleZ; // Position used for 3D texture lookup
out vec3 f_normal; // Normal vector of the particle
out vec3 f_worldPos; // World position of the particle
out float f_lifeTime; // Life Time of the particle

// This Hash Function was taken from Ben Cloward Youtube's series on Particle System in Unity/Unreal
// From 3 input numbers generates 3 pseudo-random numbers
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
    

// Returns Model View Matrix that scales particle by given scale
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

// Gets rotation matrix arround Z
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

// From given offset, scale and rotation returns BillBoard Position.
vec4 getBillboardPosition(vec3 a_positionOffset, float a_scale, float rotation){
	mat4 loc_rotMat = getRotationMatrix(rotation);
	mat4 loc_modelView = getModelView(a_scale);
	vec3 loc_vert = (inverse(u_viewMat) * loc_rotMat * vec4(in_vert, 1)).xyz; // inverse of view makes it so they are rotated to the camera. f**king pogchamp.
	
	vec4 loc_position = vec4(loc_vert + a_positionOffset, 1);
	return loc_modelView * loc_position;
}



void main(void)
{
	// Calculate index of the particle
	float loc_particleIndex = clamp(gl_InstanceID / float(u_instanceCount), 0, 1);
	f_particleIndex = loc_particleIndex;
	
	// Calculate life time
	float loc_tempLifeTime = (float(u_time)/1000) * u_particleSpeed + loc_particleIndex;
	float loc_lifeTime = (loc_tempLifeTime - floor(loc_tempLifeTime)) ;
	
	// Get important values for animations based on life time
	float loc_scale = mix(u_particleLifeSize.x, u_particleLifeSize.y, loc_lifeTime);
	float loc_velocity = mix(u_particleLifeVelocity.x, u_particleLifeVelocity.y, loc_lifeTime);
	float loc_gravity = mix(u_gravityLifeForce.x, u_gravityLifeForce.y, loc_lifeTime);
	float loc_wind = mix(u_windLifeSpeed.x, u_windLifeSpeed.y, loc_lifeTime);
	
	// Get hashed particle index used for animations
	uint loc_particleGen = uint(gl_InstanceID + loc_tempLifeTime); // Because it is put into uint, it will ignore loc_lifeTime, but it will create variations when regenerated.
	vec3 loc_hashedIndex = hash33(
	loc_particleGen,
	loc_particleGen,
	loc_particleGen);

	// Calculate Spread
	float loc_particleSpread = u_particleSpread / 180;
	vec3 loc_hashedPosition = loc_hashedIndex * 2 - vec3(1);
	
	vec3 loc_spreadPosition = loc_hashedPosition * loc_particleSpread + u_particleDirection;
	loc_spreadPosition /= length(loc_spreadPosition);
	loc_spreadPosition *= loc_velocity * (loc_lifeTime);

	// Caclulate Gravity and Wind
	vec3 loc_forcePosition = vec3(0,-1,0) * loc_gravity + u_windDirection * loc_wind;
	
	// Calculate offset based on emmitor dimensions
	vec3 loc_positionOffset = loc_hashedPosition * u_emmiterDimensions;
	
	// Calculate rotation
	float loc_rot = u_initRotation * PI / 180;
	loc_rot += (loc_lifeTime+ loc_hashedIndex.x) * u_rotationSpeed * 2 * PI;
	
	// Get Billboard position
	vec4 loc_position = getBillboardPosition(loc_positionOffset + loc_spreadPosition + loc_forcePosition, 
											loc_scale,
											loc_rot);
	
	// Set Position
	gl_Position = u_projMat * loc_position;
	
	// Set outs
	f_worldPos = loc_position.xyz;
	f_lifeTime = loc_lifeTime;
	f_texCoord = in_texCoord;
	f_fragColor = mix(u_startColor, u_endColor, loc_lifeTime);
	
	float loc_tempZ = loc_tempLifeTime - 2*(int(loc_tempLifeTime) / 2); 
	f_particleZ = loc_tempZ > 1 ? 2 - loc_tempZ : loc_tempZ;
		
	vec4 loc_normalOffset = normalize(inverse(u_viewMat) * vec4(in_vert,0)) * u_normalSpread;
	vec4 loc_normal = inverse(u_viewMat) * (vec4(0,0,1,0)) + vec4(loc_normalOffset.xyz, 0);
	f_normal = normalize(loc_normal.xyz);
}

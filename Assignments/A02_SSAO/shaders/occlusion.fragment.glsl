#version 430 core

layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_position;
layout(binding = 3) uniform sampler1D u_samples;

uniform uint u_sampleCount;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

in vec2 texCoords;
//out vec4 out_fragColor;
out float out_occlusion;

// This is from my personal computations on paper.
// It creates a matrix that converts to a 3d space with orthogonal base and has third basis vector the normal vector.
mat3 GetNormalSpaceMatrix(vec3 normalVec) {
    float n1 = normalVec.x;
    float n2 = normalVec.y;
    float n3 = normalVec.z;
    float n1s = n1 * n1;
    float n3m2 = n3 - n2;
    float nm2m3 = -n2 - n3;
    
    return mat3(
        normalize(vec3(n3m2, n1, -n1)),
        normalize(vec3(n1s * nm2m3, n3 * n3m2 + n1s, n1s - n2 * n3m2)),
        normalVec
    );
}

void main() {
    int notOccludedSamples = 0;
    
    // Get Position and Normal Info
    vec3 position = texture(u_position, texCoords).xyz;
    vec3 positionNormal = texture(u_normal, texCoords).xyz; 
    mat3 normalSpaceMat = GetNormalSpaceMatrix(positionNormal); 
    
    for (uint i = 0; i < u_sampleCount; ++i) {
        // Load sample position
        vec3 sampleOffset = texelFetch(u_samples, int(i), 0).xyz;
        sampleOffset = normalSpaceMat * sampleOffset; 
        vec3 samplePosition = position + sampleOffset;
        
        // Get Sample in Screen Space
        vec4 sampleView = u_viewMat * vec4(samplePosition, 1.0);
        vec4 sampleScreen = u_projMat * sampleView;
        
        vec3 sampleNDC = sampleScreen.xyz / sampleScreen.w;
        
        // Convert NDC to texture coordinates
        vec2 sampleTexCoords = sampleNDC.xy * 0.5 + 0.5;
        
        // Get Position to Test With in Screen Space
        vec3 testPosition = texture(u_position, sampleTexCoords).xyz;
        vec4 testView = u_viewMat * vec4(testPosition, 1.0);
        vec4 testScreen = u_projMat * testView;
        
        // Check if sample is occluded
        if (sampleScreen.z <= testScreen.z + 0.1 || length(testPosition - samplePosition) > 1) {
            notOccludedSamples++;
        }
    }
    //out_fragColor = texture(u_diffuse, texCoords);
    //out_fragColor = texture(u_diffuse, texCoords) * float(notOccludedSamples) / float(u_sampleCount);
	//out_fragColor = vec4(1,1,1,1) * float(notOccludedSamples) / float(u_sampleCount);
	out_occlusion = float(notOccludedSamples) / float(u_sampleCount);
}
#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

// Input and output images
layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

const float kernel[9] = float[](
		0.0625, 0.125, 0.0625,
		0.125,  0.25,  0.125,
		0.0625, 0.125, 0.0625
		);

void main() {
	ivec2 texSize = imageSize(inputImage);
	ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

	if (gid.x >= texSize.x || gid.y >= texSize.y) {
		return; // Skip out-of-bounds work items
	}

	vec4 sum = vec4(0.0);
	int k = 0;
	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++, k++) {
			ivec2 coord = gid + ivec2(i, j);
			// Read the texel, handle borders by clamping to edge
			vec4 color = imageLoad(inputImage, clamp(coord, ivec2(0), texSize - 1));
			sum += color * kernel[k];
		}
	}

	// Write the result to the output image
	imageStore(outputImage, gid, sum);
}

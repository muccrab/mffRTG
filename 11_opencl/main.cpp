#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
#include <clFFT.h>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

// Function to load a black and white image using stb_image
unsigned char* load_image(const char* filename, int* width, int* height, int* channels) {
	return stbi_load(filename, width, height, channels, 1); // Load as grayscale image
}

// Function to save an image using stb_image_write
void save_image(const char* filename, unsigned char* data, int width, int height, int channels) {
	stbi_write_png(filename, width, height, channels, data, width * channels); // Save as PNG image
}


void init_opencl(cl_context* context, cl_device_id* device, cl_command_queue* queue) {
	// Get platform and device information
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, device, NULL);

	// Create OpenCL context
	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		0
	};
	*context = clCreateContext(properties, 1, device, NULL, NULL, NULL);

	// Create command queue with properties
	cl_command_queue_properties queue_properties = 0;
	*queue = clCreateCommandQueueWithProperties(*context, *device, &queue_properties, NULL);
}

float blur_kernel_data[5][5] = {
    {0.003765, 0.015019, 0.023792, 0.015019, 0.003765},
    {0.015019, 0.059912, 0.094907, 0.059912, 0.015019},
    {0.023792, 0.094907, 0.150342, 0.094907, 0.023792},
    {0.015019, 0.059912, 0.094907, 0.059912, 0.015019},
    {0.003765, 0.015019, 0.023792, 0.015019, 0.003765}
};


int main() {
	// Load the image
	int width, height, channels;
	unsigned char* image_data = load_image("input_image.png", &width, &height, &channels);
	if (image_data == NULL) {
		printf("Error loading image!\n");
		return 1;
	}

	// Initialize OpenCL
	cl_context context;
	cl_device_id device;
	cl_command_queue queue;
	init_opencl(&context, &device, &queue);

	// Create OpenCL buffer for the image data
	cl_mem image_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			width * height * sizeof(float), image_data, NULL);

	// Create clFFT plan for 2D FFT
	clfftPlanHandle plan;
	clfftSetupData fftSetup;
	clfftInitSetupData(&fftSetup);
	clfftSetup(&fftSetup);
	size_t clLengths[2] = { size_t(width), size_t(height) };
	clfftCreateDefaultPlan(&plan, context, CLFFT_2D, clLengths);

	// Set plan parameters
	clfftSetPlanPrecision(plan, CLFFT_SINGLE);
	clfftSetLayout(plan, CLFFT_REAL, CLFFT_COMPLEX_INTERLEAVED);
	clfftSetResultLocation(plan, CLFFT_OUTOFPLACE);

	// Bake the plan
	clfftBakePlan(plan, 1, &queue, NULL, NULL);

	// Execute 2D FFT on the image
	cl_event fft_event;
	clfftEnqueueTransform(plan, CLFFT_FORWARD, 1, &queue, 1, &fft_event, NULL, &image_buffer, NULL, NULL);

	// Wait for FFT to complete
	clWaitForEvents(1, &fft_event);

	// Convolve the image with a blurring kernel (e.g., Gaussian kernel) in Fourier domain
	// (This is a placeholder, you need to provide your own convolution kernel)
	// For simplicity, we assume a 5x5 kernel here
	cl_mem kernel_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			5 * 5 * sizeof(float), blur_kernel_data, NULL);
	clfftEnqueueComplexMultiply(plan, 1, &queue, &fft_event, &image_buffer, &kernel_buffer, &image_buffer, 0, NULL, NULL);

	// Execute 2D IFFT on the result
	cl_event ifft_event;
	clfftEnqueueTransform(plan, CLFFT_BACKWARD, 1, &queue, 1, &ifft_event, NULL, &image_buffer, NULL, NULL);

	// Wait for IFFT to complete
	clWaitForEvents(1, &ifft_event);

	// Read the result back to host memory
	clEnqueueReadBuffer(queue, image_buffer, CL_TRUE, 0, width * height * sizeof(float), image_data, 0, NULL, NULL);

	// Normalize the result
	float max_value = 0.0f;
	for (int i = 0; i < width * height; i++) {
		if (image_data[i] > max_value) {
			max_value = image_data[i];
		}
	}
	for (int i = 0; i < width * height; i++) {
		image_data[i] = (unsigned char)(255.0f * image_data[i] / max_value);
	}

	// Save the resulting image
	save_image("output_image.png", image_data, width, height, channels);

	// Clean up
	clfftDestroyPlan(&plan);
	clfftTeardown();

	clReleaseMemObject(image_buffer);
	clReleaseMemObject(kernel_buffer);

	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	stbi_image_free(image_data);

	return 0;
}


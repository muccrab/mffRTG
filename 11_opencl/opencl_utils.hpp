// opencl_utils.hpp
#pragma once

#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <memory>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

struct Image {
    std::vector<float> data;
    int width;
    int height;
    int channels;
};

// Function to load and save images would still use stb_image.h and stb_image_write.h

std::unique_ptr<Image> load_image(const std::string& filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 1);
    if (!data) {
        std::cerr << "Error loading image.\n";
        return nullptr;
    }

    std::unique_ptr<Image> img = std::make_unique<Image>();
    img->data.resize(width * height);
    img->width = width;
    img->height = height;
    img->channels = 1; // Grayscale

    for (int i = 0; i < width * height; i++) {
        img->data[i] = static_cast<float>(data[i]);
    }

    stbi_image_free(data);
    return img;
}

void save_image(const std::string& filename, const Image& img) {
    std::vector<unsigned char> output(img.data.size());
    for (size_t i = 0; i < img.data.size(); i++) {
        output[i] = static_cast<unsigned char>(img.data[i]);
    }
    stbi_write_png(filename.c_str(), img.width, img.height, img.channels, output.data(), img.width * img.channels);
}

cl::Context createContext() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    auto device = devices.front();

    return cl::Context(device);
}

cl::CommandQueue createQueue(const cl::Context& context, const cl::Device& device) {
    return cl::CommandQueue(context, device);
}


// Prepare and return the Fourier transform of a convolution kernel
cl::Buffer prepareKernelFFT(const cl::Context& context, const cl::CommandQueue& queue, const std::vector<float>& kernel, int kernel_width, int kernel_height, int width, int height) {
    // Pad kernel to match image size
    std::vector<float> padded_kernel(width * height, 0.0f);
    int start_x = (width - kernel_width) / 2;
    int start_y = (height - kernel_height) / 2;
    for (int y = 0; y < kernel_height; ++y) {
        for (int x = 0; x < kernel_width; ++x) {
            padded_kernel[(start_y + y) * width + (start_x + x)] = kernel[y * kernel_width + x];
        }
    }

    cl::Buffer kernelBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, width * height * sizeof(float), padded_kernel.data());

    // Set up FFT plan and execute
    cl::Buffer kernelFFTBuffer = executeFFT(context, queue, kernelBuffer, width, height, true); // Assuming executeFFT() is defined to perform FFT
    return kernelFFTBuffer;
}


// Execute FFT or IFFT
cl::Buffer executeFFT(const cl::Context& context, const cl::CommandQueue& queue, const cl::Buffer& inputBuffer, int width, int height, bool forward) {
    clfftSetupData fftSetup;
    clfftInitSetupData(&fftSetup);
    clfftSetup(&fftSetup);

    clfftPlanHandle plan;
    size_t clLengths[2] = { static_cast<size_t>(width), static_cast<size_t>(height) };
    clfftCreateDefaultPlan(&plan, context(), CLFFT_2D, clLengths);

    clfftSetPlanPrecision(plan, CLFFT_SINGLE);
    clfftSetLayout(plan, CLFFT_REAL, CLFFT_HERMITIAN_INTERLEAVED);
    clfftSetResultLocation(plan, CLFFT_INPLACE);
    clfftBakePlan(plan, 1, &queue(), nullptr, nullptr);

    // Execute the FFT or IFFT
    cl::Event fftEvent;
    clfftDirection dir = forward ? CLFFT_FORWARD : CLFFT_BACKWARD;
    clfftEnqueueTransform(plan, dir, 1, &queue(), 0, nullptr, &fftEvent(), &inputBuffer(), nullptr, nullptr);
    fftEvent.wait();

    clfftDestroyPlan(&plan);
    clfftTeardown();

    return inputBuffer;  // Returning the same buffer since CLFFT_INPLACE was used
}

// Full FFT convolution process
std::unique_ptr<Image> performFFTConvolution(const cl::Context& context, const cl::CommandQueue& queue, const Image& img, const cl::Buffer& kernelFFT, int width, int height) {
    // Pad image to appropriate size
    std::vector<float> padded_image(width * height, 0.0f);
    std::copy_n(img.data.begin(), img.width * img.height, padded_image.begin() + width * (height - img.height) / 2 + (width - img.width) / 2);

    cl::Buffer imageBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, width * height * sizeof(float), padded_image.data());
    cl::Buffer imageFFTBuffer = executeFFT(context, queue, imageBuffer, width, height, true);

    // Convolution (multiplication in frequency domain)
    cl::Buffer convolvedBuffer = multiplyFrequencyDomain(context, queue, imageFFTBuffer, kernelFFT, width * height);  // Assuming this function is defined

    // Inverse FFT
    cl::Buffer resultBuffer = executeFFT(context, queue, convolvedBuffer, width, height, false);

    // Read back the result
    std::vector<float> result(width * height);
    queue.enqueueReadBuffer(resultBuffer, CL_TRUE, 0, width * height * sizeof(float), result.data());

    // Crop to original size and normalize if necessary
    std::unique_ptr<Image> output = std::make_unique<Image>();
    output->width = img.width;
    output->height = img.height;
    output->channels = img.channels;
    output->data.resize(img.width * img.height);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            output->data[y * img.width + x] = result[(y + (height - img.height) / 2) * width + x + (width - img.width) / 2];
        }
    }

    return output;
}

// Helper function to perform element-wise complex multiplication
const char* complexMultiplyKernel =
R"(
kernel void complexMultiply(global float2* a, global float2* b, global float2* result, int numElements) {
    int id = get_global_id(0);
    if (id < numElements) {
        float2 af = a[id];
        float2 bf = b[id];
        result[id] = (float2)(af.x * bf.x - af.y * bf.y, af.x * bf.y + af.y * bf.x);
    }
}
)";

cl::Buffer multiplyFrequencyDomain(const cl::Context& context, const cl::CommandQueue& queue, const cl::Buffer& fftImage, const cl::Buffer& fftKernel, int numElements) {
    cl::Program::Sources source;
    source.push_back({complexMultiplyKernel, strlen(complexMultiplyKernel)});
    cl::Program program(context, source);
    program.build("-cl-std=CL2.0");

    cl::Kernel kernel(program, "complexMultiply");
    kernel.setArg(0, fftImage);
    kernel.setArg(1, fftKernel);
    kernel.setArg(2, fftImage); // In-place multiplication
    kernel.setArg(3, numElements);

    cl::NDRange globalSize(numElements);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);

    return fftImage;  // Since the operation is in-place, return the modified image buffer
}

#include <CL/cl.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

const int width = 256;
const int height = 256;
const int iterations = 100;

const char* kernelSource = R"CLC(
__kernel void game_of_life(__global uchar* current, __global uchar* next, int width, int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    int num_neighbors = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + width) % width;
            int ny = (y + dy + height) % height;
            num_neighbors += current[ny * width + nx];
        }
    }

    int current_cell = current[y * width + x];
    next[y * width + x] = (current_cell && (num_neighbors == 2 || num_neighbors == 3)) || (!current_cell && num_neighbors == 3);
}
)CLC";

void save_png(const std::vector<cl_uchar>& buffer, int width, int height, int iteration) {
	std::vector<cl_uchar> tmp(buffer.size());
	std::transform(buffer.begin(), buffer.end(), tmp.begin(), [](const auto &val) { return val * 255; });
    std::ostringstream filename;
    filename << "output/output_" << std::setw(3) << std::setfill('0') << iteration << ".png";
    stbi_write_png(filename.str().c_str(), width, height, 1, tmp.data(), width);
}

int main() {
    // Initialize OpenCL context, device, and command queue
    cl::Platform platform = cl::Platform::getDefault();
    cl::Device device = cl::Device::getDefault();
    cl::Context context({ device });
    cl::Program program(context, kernelSource);
    program.build({ device });
    cl::CommandQueue queue(context, device);

    // Create random initial buffer
    std::vector<cl_uchar> buffer(width * height);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    for (auto& cell : buffer) {
        cell = dis(gen);
    }

    // Create OpenCL buffers
    cl::Buffer currentBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, buffer.size() * sizeof(cl_uchar), buffer.data());
    cl::Buffer nextBuffer(context, CL_MEM_READ_WRITE, buffer.size() * sizeof(cl_uchar));

    // Set kernel arguments
    cl::Kernel kernel(program, "game_of_life");
    kernel.setArg(0, currentBuffer);
    kernel.setArg(1, nextBuffer);
    kernel.setArg(2, width);
    kernel.setArg(3, height);

    for (int i = 0; i < iterations; ++i) {
        // Execute kernel
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
        queue.finish();

        // Read result
        queue.enqueueReadBuffer(nextBuffer, CL_TRUE, 0, buffer.size() * sizeof(cl_uchar), buffer.data());

        // Save to PNG
        save_png(buffer, width, height, i);

        // Swap buffers
        std::swap(currentBuffer, nextBuffer);
        kernel.setArg(0, currentBuffer);
        kernel.setArg(1, nextBuffer);
    }

    return 0;
}


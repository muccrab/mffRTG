#include "opencl_utils.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

int main() {
    auto img = load_image("input_image.png");
    if (!img) {
        std::cerr << "Failed to load image.\n";
        return 1;
    }

    auto context = createContext();
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    auto device = devices.front();
    auto queue = createQueue(context, device);

    cl::Buffer imageBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, img->width * img->height * sizeof(float), img->data.data());

// Load or define the kernel
    std::vector<float> kernel = {/* kernel data */};
    int kernel_width = 5; // Example
    int kernel_height = 5; // Example

    auto kernelFFT = prepareKernelFFT(context, queue, kernel, kernel_width, kernel_height, img->width, img->height);
    auto resultImg = performFFTConvolution(context, queue, *img, kernelFFT, img->width, img->height);

    // Save the convolved image
    save_image("convolved_output.png", *resultImg);    // Setting up and executing FFT (similar functions would be implemented in the utility header)

    return 0;
}


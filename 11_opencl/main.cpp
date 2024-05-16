
#include <CL/cl.hpp>
#include <clFFT.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// Initialize OpenCL context and queue
cl::Context createContext();
cl::CommandQueue createQueue(const cl::Context& context);

std::string getErrorDescription(cl_int errorCode);

// Perform FFT and IFFT
void performFFT(const cl::Context& context, cl::CommandQueue& queue, cl::Buffer& buffer, int width, int height, bool forward);
void saveImage(const char* filename, std::vector<float>& data, int width, int height) {
    std::vector<unsigned char> output(data.size());
    float max_value = *std::max_element(data.begin(), data.end());
    for (size_t i = 0; i < data.size(); i++) {
        float logValue = log(1.0f + data[i]);
        output[i] = static_cast<unsigned char>(255.0f * logValue / log(1.0f + max_value));
    }
    stbi_write_png(filename, width, height, 1, output.data(), width);
}

// Load image, perform FFT, save amplitude, perform IFFT, and save result
int main() {
    int width, height, channels;
    const char* inputFilename = "input_image.png";
    const char* outputFilename = "output_image.png";

    // Load the image using stb_image
    unsigned char* input = stbi_load(inputFilename, &width, &height, &channels, 0);
    if (!input) {
        std::cerr << "Failed to load the image\n";
        return -1;
    }

    std::cout << channels << "\n";
    // Convert the image data to float
    std::vector<float> imageData(width * height);
    for (int i = 0; i < width * height; ++i) {
        imageData[i] = static_cast<float>(input[i * channels]);  // Assuming grayscale conversion
    }
    stbi_image_free(input);
    saveImage("input_test.png", imageData, width, height);

    auto context = createContext();
    auto queue = createQueue(context);

    cl::Buffer imageBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, imageData.size() * sizeof(float), imageData.data());

    // Perform forward FFT
    performFFT(context, queue, imageBuffer, width, height, true);

    // Compute amplitude and save
    std::vector<float> amplitude(width * height);
    queue.enqueueReadBuffer(imageBuffer, CL_TRUE, 0, amplitude.size() * sizeof(float), amplitude.data());
    for (size_t i = 0; i < amplitude.size(); ++i) {
        amplitude[i] = std::log1p(std::sqrt(amplitude[i])); // Convert to logarithmic scale for visualization
    }
    // stbi_write_png("fft_amplitude.png", width, height, 1, amplitude.data(), width);
    saveImage("fft_amplitude.png", amplitude, width, height);

    // Perform inverse FFT
    performFFT(context, queue, imageBuffer, width, height, false);

    // Read back the result
    queue.enqueueReadBuffer(imageBuffer, CL_TRUE, 0, imageData.size() * sizeof(float), imageData.data());
    stbi_write_png(outputFilename, width, height, 1, imageData.data(), width);

    return 0;
}

cl::Context createContext() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    return cl::Context(devices.front());
}

cl::CommandQueue createQueue(const cl::Context& context) {
    return cl::CommandQueue(context, context.getInfo<CL_CONTEXT_DEVICES>().front());
}

void performFFT(const cl::Context& context, cl::CommandQueue& queue, cl::Buffer& buffer, int width, int height, bool forward) {
	clfftStatus status;
    clfftSetupData fftSetup;
    status = clfftInitSetupData(&fftSetup);
    std::cout << __LINE__ << ": " << getErrorDescription(status) << "\n";
    clfftSetup(&fftSetup);

    clfftPlanHandle plan;
    size_t clLengths[2] = { width, height };
    status = clfftCreateDefaultPlan(&plan, context(), CLFFT_2D, clLengths);
    std::cout << __LINE__ << ": " << status << "- " << getErrorDescription(status) << "\n";

    status = clfftSetPlanPrecision(plan, CLFFT_SINGLE);
    std::cout << __LINE__ << ": " << status << "- " << getErrorDescription(status) << "\n";
    status = clfftSetLayout(plan, forward ? CLFFT_REAL : CLFFT_HERMITIAN_INTERLEAVED,
                          forward ? CLFFT_HERMITIAN_INTERLEAVED : CLFFT_REAL);
    std::cout << __LINE__ << ": " << status << "- " << getErrorDescription(status) << "\n";
    status = clfftSetResultLocation(plan, CLFFT_INPLACE);
    std::cout << __LINE__ << ": " << status << "- " << getErrorDescription(status) << "\n";
    status = clfftBakePlan(plan, 1, &queue(), nullptr, nullptr);
    std::cout << __LINE__ << ": " << getErrorDescription(status) << "\n";

    cl::Event fftEvent;
    clfftDirection dir = forward ? CLFFT_FORWARD : CLFFT_BACKWARD;
    status = clfftEnqueueTransform(plan, dir, 1, &queue(), 0, nullptr, &fftEvent(), &buffer(), nullptr, nullptr);
    std::cout << __LINE__ << ": " << getErrorDescription(status) << "\n";
    fftEvent.wait();

    clfftDestroyPlan(&plan);
    clfftTeardown();
}

std::string getErrorDescription(cl_int errorCode) {
    switch (errorCode) {
        // OpenCL Error Codes
        case CL_SUCCESS:
            return "CL_SUCCESS: Success!";
        case CL_DEVICE_NOT_FOUND:
            return "CL_DEVICE_NOT_FOUND: No OpenCL devices that matched given device type were found.";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE: A device is currently not available even though it was found.";
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE: Compiler is not available.";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE: Failed to allocate memory for buffer object.";
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES: Failed to allocate resources required by the OpenCL implementation on the device.";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY: Failed to allocate resources required by the OpenCL implementation on the host.";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE: Profiling information not available.";
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP: Overlap detected in the memory copy operation.";
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH: Mismatch detected in the image format.";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED: Image format is not supported.";
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE: Program build failure.";
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE: Map failure.";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET: Sub-buffer offset alignment error.";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: Execution status error for events in wait list.";
        case CL_COMPILE_PROGRAM_FAILURE:
            return "CL_COMPILE_PROGRAM_FAILURE: Program compilation failure.";
        case CL_LINKER_NOT_AVAILABLE:
            return "CL_LINKER_NOT_AVAILABLE: Linker is not available.";
        case CL_LINK_PROGRAM_FAILURE:
            return "CL_LINK_PROGRAM_FAILURE: Program linking failure.";
        case CL_DEVICE_PARTITION_FAILED:
            return "CL_DEVICE_PARTITION_FAILED: Device partition failed.";
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
            return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE: Kernel argument information is not available.";

        // clFFT-specific Error Codes
        case CLFFT_INVALID_GLOBAL_WORK_SIZE:
            return "CLFFT_INVALID_GLOBAL_WORK_SIZE: Invalid global work size.";
        case CLFFT_INVALID_MIP_LEVEL:
            return "CLFFT_INVALID_MIP_LEVEL: Invalid mip-map level.";
        case CLFFT_INVALID_BUFFER_SIZE:
            return "CLFFT_INVALID_BUFFER_SIZE: Invalid buffer size.";
        case CLFFT_INVALID_GL_OBJECT:
            return "CLFFT_INVALID_GL_OBJECT: Invalid OpenGL object.";
        case CLFFT_INVALID_OPERATION:
            return "CLFFT_INVALID_OPERATION: Operation not allowed in current configuration.";
        case CLFFT_INVALID_EVENT:
            return "CLFFT_INVALID_EVENT: Invalid event.";
        case CLFFT_INVALID_EVENT_WAIT_LIST:
            return "CLFFT_INVALID_EVENT_WAIT_LIST: Invalid event wait list.";
        case CLFFT_INVALID_GLOBAL_OFFSET:
            return "CLFFT_INVALID_GLOBAL_OFFSET: Invalid global offset.";
        case CLFFT_INVALID_WORK_ITEM_SIZE:
            return "CLFFT_INVALID_WORK_ITEM_SIZE: Invalid work item size.";
        case CLFFT_INVALID_WORK_GROUP_SIZE:
            return "CLFFT_INVALID_WORK_GROUP_SIZE: Invalid work group size.";
        case CLFFT_INVALID_WORK_DIMENSION:
            return "CLFFT_INVALID_WORK_DIMENSION: Invalid work dimension.";
        case CLFFT_INVALID_KERNEL_ARGS:
            return "CLFFT_INVALID_KERNEL_ARGS: Invalid kernel arguments.";
        case CLFFT_INVALID_ARG_SIZE:
            return "CLFFT_INVALID_ARG_SIZE: Invalid argument size.";
        case CLFFT_INVALID_ARG_VALUE:
            return "CLFFT_INVALID_ARG_VALUE: Invalid argument value.";
        case CLFFT_INVALID_ARG_INDEX:
            return "CLFFT_INVALID_ARG_INDEX: Invalid argument index.";
        case CLFFT_INVALID_KERNEL:
            return "CLFFT_INVALID_KERNEL: Invalid kernel.";
        case CLFFT_INVALID_KERNEL_DEFINITION:
            return "CLFFT_INVALID_KERNEL_DEFINITION: Invalid kernel definition.";
        case CLFFT_INVALID_KERNEL_NAME:
            return "CLFFT_INVALID_KERNEL_NAME: Invalid kernel name.";
        case CLFFT_INVALID_PROGRAM_EXECUTABLE:
            return "CLFFT_INVALID_PROGRAM_EXECUTABLE: Invalid program executable.";
        case CLFFT_INVALID_PROGRAM:
            return "CLFFT_INVALID_PROGRAM: Invalid program.";
        case CLFFT_INVALID_BUILD_OPTIONS:
            return "CLFFT_INVALID_BUILD_OPTIONS: Invalid build options.";
        case CLFFT_INVALID_BINARY:
            return "CLFFT_INVALID_BINARY: Invalid binary.";
        case CLFFT_INVALID_SAMPLER:
            return "CLFFT_INVALID_SAMPLER: Invalid sampler.";
        case CLFFT_INVALID_IMAGE_SIZE:
            return "CLFFT_INVALID_IMAGE_SIZE: Invalid image size.";
        case CLFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CLFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR: Invalid image format descriptor.";
        case CLFFT_INVALID_MEM_OBJECT:
            return "CLFFT_INVALID_MEM_OBJECT: Invalid memory object.";
        case CLFFT_INVALID_HOST_PTR:
            return "CLFFT_INVALID_HOST_PTR: Invalid host pointer.";
        case CLFFT_INVALID_COMMAND_QUEUE:
            return "CLFFT_INVALID_COMMAND_QUEUE: Invalid command queue.";
        case CLFFT_INVALID_QUEUE_PROPERTIES:
            return "CLFFT_INVALID_QUEUE_PROPERTIES: Invalid queue properties.";
        case CLFFT_INVALID_CONTEXT:
            return "CLFFT_INVALID_CONTEXT: Invalid context.";
        case CLFFT_INVALID_DEVICE:
            return "CLFFT_INVALID_DEVICE: Invalid device.";
        case CLFFT_INVALID_PLATFORM:
            return "CLFFT_INVALID_PLATFORM: Invalid platform.";
        case CLFFT_INVALID_DEVICE_TYPE:
            return "CLFFT_INVALID_DEVICE_TYPE: Invalid device type.";
        case CLFFT_INVALID_VALUE:
            return "CLFFT_INVALID_VALUE: Invalid value.";

	case CLFFT_BUGCHECK:
	    return "Bugcheck.";
	case CLFFT_NOTIMPLEMENTED:
	     return "Functionality is not implemented yet.";
	case CLFFT_TRANSPOSED_NOTIMPLEMENTED:
	    return "Transposed functionality is not implemented for this transformation.";
	case CLFFT_FILE_NOT_FOUND:
	    return "Tried to open an existing file on the host system, but failed.";
	case CLFFT_FILE_CREATE_FAILURE:
		return "Tried to create a file on the host system, but failed.";
	case CLFFT_VERSION_MISMATCH:
		return "Version conflict between client and library.";
	case CLFFT_INVALID_PLAN:
		return "Requested plan could not be found.";
	case CLFFT_DEVICE_NO_DOUBLE:
		return "Double precision not supported on this device.";
	case CLFFT_DEVICE_MISMATCH:
		return "Attempt to run on a device using a plan baked for a different device.";
	case CLFFT_ENDSTATUS:
		return "The last value of the enum, and marks the length of clfftStatus.";

        default:
            return "Unknown error code.";
    }
}


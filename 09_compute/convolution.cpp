#include <iostream>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <array>

#include "ogl_resource.hpp"
#include "error_handling.hpp"
#include "window.hpp"
#include "shader.hpp"

// #include "scene_definition.hpp"
// #include "renderer.hpp"

#include <vector>
#include <chrono>

#include "ogl_geometry_factory.hpp"
#include "ogl_material_factory.hpp"

#include <glm/gtx/string_cast.hpp>

static int const cWorkGroupSize = 256;

void compute() {
	auto timeQuery = createQuery();
	// Create compute shader
	auto convolutionShaderSource = loadShaderSource("./shaders/convolution.compute.glsl");
	auto convolutionProgram = createComputeShaderProgram(convolutionShaderSource);

	auto imageData = loadImage("./textures/test.jpg");
	OpenGLResource inputTexture = createTextureFromData(*imageData);
	OpenGLResource outputTexture = createTextureFromData(*imageData);

	GL_CHECK(glUseProgram(convolutionProgram.get()));
	GL_CHECK(glBindImageTexture(0, inputTexture.get(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F));
	GL_CHECK(glBindImageTexture(1, outputTexture.get(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F));

	GL_CHECK(glDispatchCompute((imageData->width + 15) / 16, (imageData->height + 15) / 16, 1));
	GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));


	// Create buffer
	// std::vector<int> data(1024, 1);
        //
	// size_t numWorkGroups = (data.size() + cWorkGroupSize - 1) / cWorkGroupSize;  // Round up division
	// size_t numElements = data.size();
	// std::vector<int> aux(numWorkGroups, 0);
        //
	// // Generate and bind the data buffer
	// auto dataBuffer = createBuffer();
	// // glGenBuffers(1, &dataBuffer);
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer.get());
	// glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(int), data.data(), GL_DYNAMIC_DRAW);
        //
	// // Generate and bind the auxiliary buffer
	// // glGenBuffers(1, &auxBuffer);
	// auto auxBuffer = createBuffer();
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxBuffer.get());
	// glBufferData(GL_SHADER_STORAGE_BUFFER, aux.size() * sizeof(int), aux.data(), GL_DYNAMIC_DRAW);
        //
	// // Optionally, unbind the buffer to avoid accidental modifications
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        //
	// auto start = std::chrono::high_resolution_clock::now();
	// glBeginQuery(GL_TIME_ELAPSED, timeQuery.get());
	// // Dispatch the local scan shader
	// glUseProgram(scanLocalProgram.get());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dataBuffer.get());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, auxBuffer.get());
	// glDispatchCompute((numElements + cWorkGroupSize -1) / cWorkGroupSize, 1, 1);
	// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //
	// // Dispatch the adjustment shader
	// glUseProgram(scanSumProgram.get());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dataBuffer.get());
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, auxBuffer.get());
	// glDispatchCompute(numWorkGroups, 1, 1); // num_groups is the number of groups dispatched in the first pass
	// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	// glEndQuery(GL_TIME_ELAPSED);
	// auto end = std::chrono::high_resolution_clock::now();
        //
	// GLuint64 elapsedTime;
	// glGetQueryObjectui64v(timeQuery.get(), GL_QUERY_RESULT, &elapsedTime);
        //
	// // Read back data
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer.get());
	// int* ptr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	// std::vector<int> output(ptr, ptr + data.size());
	// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	// //
	// // Read back data
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxBuffer.get());
	// int* auxPtr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	// std::vector<int> auxOutput(auxPtr, auxPtr + aux.size());
	// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        //
	// // Print time taken
	// std::chrono::duration<double, std::milli> elapsed = end - start;
	// std::cout << "Compute shader time: " << elapsed.count() << " ms" << std::endl;
	// std::cout << "Compute shader time (query): " << (elapsedTime / 1.0e6) << " ms" << std::endl;
        //
	// std::cout << "Result check: \n";
	// for (auto i : output) {
	// 	std::cout << i <<", ";
	// }
	// std::cout << "\n";
	// std::cout << "\n";
        //
	// for (auto i : auxOutput) {
	// 	std::cout << i <<", ";
	// }
	// std::cout << "\n";
	// for (auto i : {1, 1000, 9999}) {
	// 	std::cout << i << " : " << output[i] << "\n";
	// }
}

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();

		compute();
		// MouseTracking mouseTracking;
		// Camera camera(window.aspectRatio());
		// camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
		// camera.lookAt(glm::vec3());
		// window.onResize([&camera, &window](int width, int height) {
		// 		camera.setAspectRatio(window.aspectRatio());
		// 	});
                //
		// window.onCheckInput([&camera, &mouseTracking](GLFWwindow *aWin) {
		// 		mouseTracking.update(aWin);
		// 		if (glfwGetMouseButton(aWin, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// 			camera.orbit(-0.4f * mouseTracking.offset(), glm::vec3());
		// 		}
		// 		if (glfwGetKey(aWin, GLFW_KEY_ENTER) == GLFW_PRESS) {
		// 			camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
		// 			camera.lookAt(glm::vec3());
		// 		}
		// 	});

		// OGLMaterialFactory materialFactory;
		// materialFactory.loadShadersFromDir("./shaders/");
		// // materialFactory.loadTexturesFromDir("./textures/");
                //
		// OGLGeometryFactory geometryFactory;
                //
                //
		// std::array<SimpleScene, 2> scenes {
		// 	createCubeScene(materialFactory, geometryFactory),
		// };
		// int currentSceneIdx = 0;
                //
		// Renderer renderer;
                //
		// renderer.initialize();
		// window.runLoop([&] {
		// 	renderer.renderScene(scenes[currentSceneIdx], camera, RenderOptions{"solid"});
		// });
	} catch (ShaderCompilationError &exc) {
		std::cerr
			<< "Shader compilation error!\n"
			<< "Shader type: " << exc.shaderTypeName()
			<<"\nError: " << exc.what() << "\n";
		return -3;
	} catch (OpenGLError &exc) {
		std::cerr << "OpenGL error: " << exc.what() << "\n";
		return -2;
	} catch (std::exception &exc) {
		std::cerr << "Error: " << exc.what() << "\n";
		return -1;
	}

	glfwTerminate();
	return 0;
}




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

#include "simple_scene.hpp"
#include "renderer.hpp"

#include "ogl_geometry_factory.hpp"
#include "material_factory.hpp"

#include <glm/gtx/string_cast.hpp>


int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();
		MouseTracking mouseTracking;
		std::cout << "*******************\n";
		Camera camera(window.aspectRatio());
		camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
		camera.lookAt(glm::vec3());
		// camera.printInfo(std::cout);
		// std::cout << "*******************\n";
		// camera.setPosition(glm::vec3(-5.0f, 0.0f, -5.0f));
		// camera.lookAt(glm::vec3());
		// std::cout << "*******************\n";
		// // camera.setRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
		// camera.printInfo(std::cout);
                //
		// glm::vec4 newpos =  camera.getViewMatrix()*glm::vec4(0.0f, 0.0f, 0.0f, 1.0);
		// std::cout << "newpos: " << glm::to_string(newpos) << "\n";
		// glm::vec4 newpos2 =  camera.getProjectionMatrix()*camera.getViewMatrix()*glm::vec4(0.0f, 0.0f, 0.0f, 1.0);
		// std::cout << "newpos2: " << glm::to_string(newpos2) << "\n";
		// std::cout << "*******************\n";
		// camera.setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
		// camera.lookAt(glm::vec3());
                //
		window.onResize([&camera, &window](int width, int height) {
				camera.setAspectRatio(window.aspectRatio());
			});

		window.onCheckInput([&camera, &mouseTracking](GLFWwindow *aWin) {
				mouseTracking.update(aWin);
				if (glfwGetMouseButton(aWin, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					std::cout << "MOUSE DOWN " << glm::to_string(mouseTracking.offset()) << "\n";
					camera.orbit(-0.4f * mouseTracking.offset(), glm::vec3());
				}
				if (glfwGetKey(aWin, GLFW_KEY_ENTER) == GLFW_PRESS) {
					camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
					camera.lookAt(glm::vec3());
				}

			});

		OGLMaterialFactory materialFactory;
		materialFactory.loadShadersFromDir("./shaders/");
		// materialFactory.loadTexturesFromDir("./textures/");

		OGLGeometryFactory geometryFactory;


		std::array<SimpleScene, 2> scenes {
			createCubeScene(materialFactory, geometryFactory),
		};
		int currentSceneIdx = 0;

		Renderer renderer;

		for (auto &scene : scenes) {

		}

		window.runLoop([&] {
			renderer.renderScene(scenes[currentSceneIdx], camera);
		});
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




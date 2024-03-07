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

struct AppConfig {

};

void handleUserInput(AppConfig &aConfig, Window &aWindow) {


}

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();
		window.setInputHandler([&config](Window &aWindow){
			handleUserInput(window, config);
		});

		std::array<std::unique_ptr<BaseScene>> scenes {
			createCubeScene(),
			createSphereScene()
		};
		int currentSceneIdx = 0;

		Camera camera;
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




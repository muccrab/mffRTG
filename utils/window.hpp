#pragma once

#include <exception>
#include <iostream>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>

#include <glm/glm.hpp>

class Window {
public:
	Window() {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create a windowed mode window and its OpenGL context
		mWindow = glfwCreateWindow(800, 600, "OpenGL Example", NULL, NULL);
		if (!mWindow) {
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwSetWindowUserPointer(mWindow, this);

		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

		makeCurrent();
		// Load OpenGL functions using glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD");
		}

		glfwSetKeyCallback(mWindow, keyCallback);
		// glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSwapInterval(1);
	}

	~Window() {

	}

	std::array<int, 2> size() {
		std::array<int, 2> result{0};
		glfwGetWindowSize(mWindow, &(result[0]), &(result[1]));
		return result;
	}

	void makeCurrent() {
		glfwMakeContextCurrent(mWindow);
	}

	void runLoop(std::function<void(void)> loopBody) {
		while (!glfwWindowShouldClose(mWindow)) {
			processInput(mWindow);

			loopBody();

			// GLFW: swap buffers and poll IO events
			glfwSwapBuffers(mWindow);
		}
	}

	void onResize(std::function<void(int, int)> aOnResizeCallback) {
		mOnResizeCallback = aOnResizeCallback;
	}

	void onCheckInput(std::function<void(GLFWwindow*)> aCheckInput) {
		mCheckInput = aCheckInput;
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		void * ptr = glfwGetWindowUserPointer(window);
		if (ptr) {
			Window *winPtr = reinterpret_cast<Window *>(ptr);
			if (winPtr->mKeyCallback) {
				winPtr->mKeyCallback(window, key, scancode, action, mods);
			}
		}
	}

	void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> aKeyCallback) {
		mKeyCallback = aKeyCallback;
	}

	double elapsedTime() const {
		return glfwGetTime();
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		GL_CHECK(glViewport(0, 0, width, height));
		void * ptr = glfwGetWindowUserPointer(window);
		if (ptr) {
			Window *winPtr = reinterpret_cast<Window *>(ptr);
			if (winPtr->mOnResizeCallback) {
				winPtr->mOnResizeCallback(width, height);
			}
		}

	}

	void processInput(GLFWwindow* window) {
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		if (mCheckInput) {
			mCheckInput(mWindow);
		}
	}

	float aspectRatio() const {
		int width, height;
		glfwGetWindowSize(mWindow, &width, &height);
		return float(width) / height;
	}

protected:
	GLFWwindow* mWindow;

	std::function<void(int, int)> mOnResizeCallback;
	std::function<void(GLFWwindow*, int, int, int, int)> mKeyCallback;

	std::function<void(GLFWwindow*)> mCheckInput;
};

struct MouseTracking {
	void update(GLFWwindow *window) {
		previousX = currentX;
		previousY = currentY;
		glfwGetCursorPos(window, &currentX, &currentY);
	}

	void reset() {
		currentX = 0.0;
		currentY = 0.0;

		previousX = 0.0;
		previousY = 0.0;
	}

	glm::vec2 offset() {
		return glm::vec2(
			float(currentX - previousX),
			float(currentY - previousY));
	}

	double currentX = 0.0;
	double currentY = 0.0;

	double previousX = 0.0;
	double previousY = 0.0;
};

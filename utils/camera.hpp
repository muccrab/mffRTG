#pragma once


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene_object.hpp"

class Camera : public SceneObject {
public:
	Camera(float aspectRatio, float fov=45.0f, float nearPlane=0.01f, float farPlane=1000.0f)
		: SceneObject(), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {}

	glm::mat4 getViewMatrix() const {
		// Convert quaternion rotation to rotation matrix
		glm::mat4 rotationMat = glm::toMat4(rotation);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), -position) * rotationMat;
		return view;
	}

	glm::mat4 getProjectionMatrix() const {
		return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	}

	void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		glm::vec3 direction = glm::normalize(target - position);
		// Create a look at quaternion
		rotation = glm::quatLookAt(direction, up);
	}

private:
	float fov;          // Field of view in radians
	float aspectRatio;  // Aspect ratio of the viewport
	float nearPlane;    // Near clipping plane
	float farPlane;     // Far clipping plane
};

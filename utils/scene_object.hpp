#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> // For glm::quat

class SceneObject {
public:
	SceneObject()
		: position(glm::vec3(0.0f)),
		rotation(glm::quat(glm::vec3(0.0f))), // Initialize with an identity quaternion
		scale(glm::vec3(1.0f)) {}

	virtual ~SceneObject() {}

	// Setters
	void setPosition(const glm::vec3& pos) { position = pos; }
	void setRotation(const glm::quat& rot) { rotation = rot; }
	void setScale(const glm::vec3& scl) { scale = scl; }

	// Getters
	const glm::vec3& getPosition() const { return position; }
	const glm::quat& getRotation() const { return rotation; }
	const glm::vec3& getScale() const { return scale; }

protected:
	glm::vec3 position;
	glm::quat rotation; // Using quaternion for rotation
	glm::vec3 scale;
};

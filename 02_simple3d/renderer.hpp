#pragma once

#include <vector>

#include "camera.hpp"
#include "ogl_geometry_factory.hpp"


class Renderer {
public:
	template<typename TScene>
	void renderScene(const TScene &aScene, const Camera &aCamera) {
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		auto renderOptions = RenderOptions{};
		std::vector<RenderData> renderData;
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(renderOptions);
			if (data) {
				renderData.push_back(data.value());
			}
		}

		for (const auto &data: renderData) {
			const glm::mat4 &modelMat = data.modelMat;
			const MaterialParameters &params = data.mMaterialParams;
			const OGLShaderProgram &shaderProgram = static_cast<const OGLShaderProgram &>(data.mShaderProgram);
			const OGLGeometry &geometry = static_cast<const OGLGeometry&>(data.mGeometry);

			shaderProgram.use();
			shaderProgram.setUniformMatrices(
					modelMat,
					aCamera.getViewMatrix(),
					aCamera.getProjectionMatrix());
			// shaderProgram.setUniformMatrices(
			// 		glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
			// 		glm::mat4(1.0f),
			// 		glm::mat4(1.0f)
			// 		);

			geometry.bind();
			geometry.draw();
		}
	}
protected:

};

#pragma once

#include <vector>

#include "camera.hpp"
#include "ogl_geometry_factory.hpp"


class Renderer {
public:
	template<typename TScene, typename TCamera>
	void renderScene(const TScene &aScene, const TCamera &aCamera, RenderOptions aRenderOptions) {
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		std::vector<RenderData> renderData;
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(aRenderOptions);
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

			geometry.bind();
			geometry.draw();
		}
	}
protected:

};

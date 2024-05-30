#pragma once

#include <vector>

#include "camera.hpp"
#include "spotlight.hpp"
#include "framebuffer.hpp"
#include "shadowmap_framebuffer.hpp"
#include "ogl_material_factory.hpp"
#include "ogl_geometry_factory.hpp"

constexpr size_t OCCLUSION_SAMPLE_COUNT = 64;



class QuadRenderer {
public:
	QuadRenderer()
		: mQuad(generateQuadTex())
	{}

	void render(const OGLShaderProgram &aShaderProgram, MaterialParameterValues &aParameters) const {
		aShaderProgram.use();
		aShaderProgram.setMaterialParameters(aParameters, MaterialParameterValues());
		GL_CHECK(glBindVertexArray(mQuad.vao.get()));
  		GL_CHECK(glDrawElements(mQuad.mode, mQuad.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(0)));
	}
protected:

	IndexedBuffer mQuad;
};

inline std::vector<CADescription> getColorNormalPositionAttachments() {
	return {
		{ GL_RGBA, GL_FLOAT, GL_RGBA },
		// To store values outside the range [0,1] we need different internal format then normal GL_RGBA
		{ GL_RGBA, GL_FLOAT, GL_RGBA32F },
		{ GL_RGBA, GL_FLOAT, GL_RGBA32F },
	};
}

inline std::vector<CADescription> getSingleColorAttachment() {
	return {
		{ GL_RGBA, GL_FLOAT, GL_RGBA32F },
	};
}

inline std::vector<CADescription> getSingleFloatAttachment() {
	return {
		{ GL_RED, GL_FLOAT, GL_RED },
	};
}


class Renderer {
public:
	Renderer(OGLMaterialFactory &aMaterialFactory)
		: mMaterialFactory(aMaterialFactory)
	{
		mCompositingShader = std::static_pointer_cast<OGLShaderProgram>(
				mMaterialFactory.getShaderProgram("compositing"));
		// mShadowMapShader = std::static_pointer_cast<OGLShaderProgram>(
		// 	mMaterialFactory.getShaderProgram("solid_color"));
		mShadowMapShader = std::static_pointer_cast<OGLShaderProgram>(
			mMaterialFactory.getShaderProgram("shadowmap"));

		mOcclusionShader = std::static_pointer_cast<OGLShaderProgram>(
			mMaterialFactory.getShaderProgram("occlusion"));

		mOcclusionBlurShader = std::static_pointer_cast<OGLShaderProgram>(
			mMaterialFactory.getShaderProgram("occlusionblur"));
	}

	void initialize(int aWidth, int aHeight) {
		mWidth = aWidth;
		mHeight = aHeight;
		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

		mFramebuffer = std::make_unique<Framebuffer>(aWidth, aHeight, getColorNormalPositionAttachments());
		mShadowmapFramebuffer = std::make_unique<Framebuffer>(600, 600, getSingleColorAttachment());
		mComposingFramebuffer = std::make_unique<Framebuffer>(aWidth, aHeight, getSingleColorAttachment());
		mOcclusionFramebuffer = std::make_unique<Framebuffer>(aWidth, aHeight, getSingleFloatAttachment());

		// mShadowmapFramebuffer = std::make_unique<ShadowmapFramebuffer>(600, 600);
		mCompositingParameters = {
			{ "u_diffuse", TextureInfo("diffuse", mFramebuffer->getColorAttachment(0)) },
			{ "u_normal", TextureInfo("diffuse", mFramebuffer->getColorAttachment(1)) },
			{ "u_position", TextureInfo("diffuse", mFramebuffer->getColorAttachment(2)) },
			// { "u_shadowMap", TextureInfo("shadowMap", mShadowmapFramebuffer->getDepthMap()) },
			{ "u_shadowMap", TextureInfo("shadowMap", mShadowmapFramebuffer->getColorAttachment(0)) }
		};

		mOcclusionParameters = {
			{ "u_normal",  TextureInfo("diffuse", mFramebuffer->getColorAttachment(1)) },
			{ "u_position", TextureInfo("diffuse", mFramebuffer->getColorAttachment(2)) }
		};

		mOcclusionBlurParameters = {
			{ "u_diffuse", TextureInfo("diffuse", mFramebuffer->getColorAttachment(0))},
			{ "u_occlusion", TextureInfo("diffuse", mOcclusionFramebuffer->getColorAttachment(0))}
		};


		// Generate Sample Points

		mOcclusionShader->use();
		

		float sampleTextureData[3*OCCLUSION_SAMPLE_COUNT];

		srand((unsigned)time(0));
		float (*RandomFloat) () = [] {
			return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		};

		float pi = glm::pi<float>();

		for (size_t i = 0; i < OCCLUSION_SAMPLE_COUNT; ++i) {
			float r = RandomFloat();
			float a = (RandomFloat() * pi - pi/2) * 3/4;
			float b = RandomFloat() * 2 * pi;

			//sample = glm::normalize(sample);

			//sample *= RandomFloat(); // This will amke it so samples aren't only on the dome around point, but also in the dome itself.

			glm::vec3 sample = glm::vec3(
				r * glm::sin(a) * glm::cos(b),
				r * glm::sin(a) * glm::sin(b),
				r * glm::cos(a)
			);


			sampleTextureData[3 * i] = sample.x;
			sampleTextureData[3 * i + 1] = sample.y;
			sampleTextureData[3 * i + 2] = sample.z;
		}

		std::shared_ptr<OGLTexture> sampleTexture = std::make_shared<OGLTexture>(create1DFloatTexture(sampleTextureData, OCCLUSION_SAMPLE_COUNT), GL_TEXTURE_1D);
		mOcclusionParameters.insert(
			{ "u_samples", TextureInfo("float", sampleTexture) }
		);
		mOcclusionParameters.insert(
			{ "u_sampleCount", static_cast<unsigned int>(OCCLUSION_SAMPLE_COUNT) }
		);


		// Generate Noise Texture

	}

	void clear() {
		mFramebuffer->bind();
		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	template<typename TScene, typename TCamera>
	void geometryPass(const TScene &aScene, const TCamera &aCamera, RenderOptions aRenderOptions) {
		GL_CHECK(glEnable(GL_DEPTH_TEST));
		GL_CHECK(glViewport(0, 0, mWidth, mHeight));
		mFramebuffer->bind();
		mFramebuffer->setDrawBuffers();
		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		std::vector<RenderData> renderData;
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(aRenderOptions);
			if (data) {
				renderData.push_back(data.value());
			}
		}

		MaterialParameterValues fallbackParameters;
		fallbackParameters["u_projMat"] = projection;
		fallbackParameters["u_viewMat"] = view;
		fallbackParameters["u_solidColor"] = glm::vec4(0,0,0,1);
		fallbackParameters["u_viewPos"] = aCamera.getPosition();
		for (const auto &data: renderData) {
			const glm::mat4 modelMat = data.modelMat;
			const MaterialParameters &params = data.mMaterialParams;
			const OGLShaderProgram &shaderProgram = static_cast<const OGLShaderProgram &>(data.mShaderProgram);
			const OGLGeometry &geometry = static_cast<const OGLGeometry&>(data.mGeometry);

			fallbackParameters["u_modelMat"] = modelMat;
			fallbackParameters["u_normalMat"] = glm::mat3(modelMat);

			shaderProgram.use();
			shaderProgram.setMaterialParameters(params.mParameterValues, fallbackParameters);

			geometry.bind();
			geometry.draw();
		}
		mFramebuffer->unbind();
	}

	template<typename TLight>
	void compositingPass(const TLight &aLight) {

		GL_CHECK(glDisable(GL_DEPTH_TEST));
		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		mComposingFramebuffer->bind();
		mCompositingParameters["u_lightPos"] = aLight.getPosition();
		mCompositingParameters["u_lightMat"] = aLight.getViewMatrix();
		mCompositingParameters["u_lightProjMat"] = aLight.getProjectionMatrix();
		mQuadRenderer.render(*mCompositingShader, mCompositingParameters);
		mComposingFramebuffer->unbind();
	}

	template<typename TScene, typename TLight>
	void shadowMapPass(const TScene &aScene, const TLight &aLight) {
		GL_CHECK(glEnable(GL_DEPTH_TEST));
		mShadowmapFramebuffer->bind();
		GL_CHECK(glViewport(0, 0, 600, 600));
		GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		mShadowmapFramebuffer->setDrawBuffers();
		auto projection = aLight.getProjectionMatrix();
		auto view = aLight.getViewMatrix();

		MaterialParameterValues fallbackParameters;
		fallbackParameters["u_projMat"] = projection;
		fallbackParameters["u_viewMat"] = view;
		fallbackParameters["u_viewPos"] = aLight.getPosition();

		std::vector<RenderData> renderData;
		RenderOptions renderOptions = {"solid"};
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(renderOptions);
			if (data) {
				renderData.push_back(data.value());
			}
		}
		mShadowMapShader->use();
		for (const auto &data: renderData) {
			const glm::mat4 modelMat = data.modelMat;
			const MaterialParameters &params = data.mMaterialParams;
			const OGLShaderProgram &shaderProgram = static_cast<const OGLShaderProgram &>(data.mShaderProgram);
			const OGLGeometry &geometry = static_cast<const OGLGeometry&>(data.mGeometry);

			fallbackParameters["u_modelMat"] = modelMat;
			fallbackParameters["u_normalMat"] = glm::mat3(modelMat);

			mShadowMapShader->setMaterialParameters(fallbackParameters, {});

			geometry.bind();
			geometry.draw();
		}



		mShadowmapFramebuffer->unbind();
	}

	template<typename TCamera>
	void occlusionAfterEffect(const TCamera& aCamera) {
		mOcclusionFramebuffer->bind();
		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		GL_CHECK(glDisable(GL_DEPTH_TEST));
		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

		mOcclusionParameters["u_projMat"] = projection;
		mOcclusionParameters["u_viewMat"] = view;

		mQuadRenderer.render(*mOcclusionShader, mOcclusionParameters);
		mOcclusionFramebuffer->unbind();
	}


	void occlusionBlurrEffect(int blurStrength) {


		GL_CHECK(glDisable(GL_DEPTH_TEST));
		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

		mOcclusionBlurParameters["u_blurStrength"] = blurStrength;

		mQuadRenderer.render(*mOcclusionBlurShader, mOcclusionBlurParameters);
	}



protected:
	int mWidth = 100;
	int mHeight = 100;
	std::unique_ptr<Framebuffer> mFramebuffer;
	std::unique_ptr<Framebuffer> mShadowmapFramebuffer;
	std::unique_ptr<Framebuffer> mComposingFramebuffer;
	std::unique_ptr<Framebuffer> mOcclusionFramebuffer;
	// std::unique_ptr<ShadowmapFramebuffer> mShadowmapFramebuffer;
	MaterialParameterValues mCompositingParameters;
	MaterialParameterValues mOcclusionParameters;
	MaterialParameterValues mOcclusionBlurParameters;
	QuadRenderer mQuadRenderer;
	std::shared_ptr<OGLShaderProgram> mCompositingShader;
	std::shared_ptr<OGLShaderProgram> mShadowMapShader;
	std::shared_ptr<OGLShaderProgram> mOcclusionShader;
	std::shared_ptr<OGLShaderProgram> mOcclusionBlurShader;
	OGLMaterialFactory &mMaterialFactory;


private:
	inline OpenGLResource create1DFloatTexture(const float* imgData, int size) {
		auto textureID = createTexture();
		GL_CHECK(glBindTexture(GL_TEXTURE_1D, textureID.get()));

		// Set texture wrapping and filtering options
		GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));


		//GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, imgData.width, imgData.height, 0, format, GL_UNSIGNED_BYTE, imgData.data.get()));
		GL_CHECK(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16F, size, 0, GL_RGB, GL_FLOAT, imgData));
		GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));

		return textureID;
	}

	inline OpenGLResource createTexture() {
		return OpenGLResource(
			[] {
				GLuint id = 0;
				GL_CHECK(glGenTextures(1, &id));
				return id;
			},
			[](GLuint id) {
				glDeleteTextures(1, &id);
			});
	}

};

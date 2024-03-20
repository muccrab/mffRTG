#pragma once

#include <glad/glad.h>

struct AttachmentDescription {

}

template<GLenum tFormat, GLenum tType, GLint tInternalFormat>
struct CADescription {
	static constexpr GLenum Format = tFormat;
	static constexpr GLenum Type = tType;
	static constexpr GLint InternalFormat = tInternalFormat;
};

template<typename... TDescriptions>
struct ColorAttachmentDescriptions {
	static constexpr int count = sizeof...(Descriptions);
};

template<typename TColorAttachmentDescriptions>
class Framebuffer {
public:
	static constexpr int cColorAttachmentCount = TColorAttachmentDescriptions::count;

	Framebuffer()
	{

	}

	init

	OpenGLResource createColorAttachment(
			int aAttachmentIndex,
			int aWidth,
			int aHeight,
			GLint aInternalFormat = GL_RGBA,
			GLenum aFormat = GL_RGBA,
			GLenum aType = GL_FLOAT)
	{
		auto textureID = createTexture();
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID.get()));
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, aInternalFormat, aWidth, aHeight, 0, aFormat, aType, NULL));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		// Attach the texture to the FBO
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + aAttachmentIndex, GL_TEXTURE_2D, textureID.get(), 0));
		return textureID;
	}

	OpenGLResource createDepthAndStencilBuffers() {
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	}

	void setDrawBuffers() {
		GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, drawBuffers);

	}

	void checkStatus() {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer is not complete!" << std::endl;
		}
	}


	std::array<OpenGLResource, cColorAttachmentCount> mColorTextures;


};

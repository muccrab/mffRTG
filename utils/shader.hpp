#pragma once

#include <sstream>
#include "ogl_resource.hpp"
#include "error_handling.hpp"

inline std::string getShaderTypeName(GLenum aShaderType) {
    switch (aShaderType) {
        case GL_VERTEX_SHADER:
            return "Vertex Shader";
        case GL_FRAGMENT_SHADER:
            return "Fragment Shader";
        case GL_GEOMETRY_SHADER:
            return "Geometry Shader";
        #ifdef GL_COMPUTE_SHADER
        case GL_COMPUTE_SHADER:
            return "Compute Shader";
        #endif
        #ifdef GL_TESS_CONTROL_SHADER
        case GL_TESS_CONTROL_SHADER:
            return "Tessellation Control Shader";
        #endif
        #ifdef GL_TESS_EVALUATION_SHADER
        case GL_TESS_EVALUATION_SHADER:
            return "Tessellation Evaluation Shader";
        #endif
        default:
            return "Unknown Shader Type";
    }
}


class ShaderCompilationError: public OpenGLError {
public:
	ShaderCompilationError(const std::string& message, GLenum aShaderType)
		: OpenGLError(message)
		, mShaderType(aShaderType)
	{}

	std::string shaderTypeName() const {
		return getShaderTypeName(mShaderType);
	}

	GLenum shaderType() const {
		return mShaderType;
	}
protected:
	GLenum mShaderType;
};

inline std::string getShaderInfoLog(GLuint shader) {
	GLint logLength = 0;
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));

	std::vector<char> log(logLength);
	if (logLength > 0) {
		GL_CHECK(glGetShaderInfoLog(shader, logLength, nullptr, log.data()));
		return std::string(log.begin(), log.end());
	}

	return {};
}

inline auto compileShader(GLenum aShaderType, const std::string& aSource) {
	auto shader = createShader(aShaderType);
	const char* src = aSource.c_str();
	GL_CHECK(glShaderSource(shader.get(), 1, &src, nullptr));
	GL_CHECK(glCompileShader(shader.get()));

	// Error handling
	int result;
	GL_CHECK(glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		throw ShaderCompilationError(getShaderInfoLog(shader.get()), aShaderType);
	}

	return shader;
}

inline auto createShaderProgram(const OpenGLResource& vertexShader, OpenGLResource& fragmentShader) {
	auto program = createShaderProgram();
	GL_CHECK(glAttachShader(program.get(), vertexShader.get()));
	GL_CHECK(glAttachShader(program.get(), fragmentShader.get()));
	GL_CHECK(glLinkProgram(program.get()));
	GL_CHECK(glValidateProgram(program.get()));
	// TODO - check validation result
	return program;
}

inline auto createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
	auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	return createShaderProgram(vs, fs);
}



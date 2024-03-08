#pragma once

#include <filesystem>
#include <regex>
#include <map>
#include <variant>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"

namespace fs = std::filesystem;


struct TextureName {
	std::string name;
};

using MaterialParam = std::variant<float, glm::vec2, glm::vec3, glm::vec4, TextureName>;
using MaterialParameterValues = std::map<std::string, MaterialParam>;

enum class RenderStyle {
	Solid,
	Lines
};

class MaterialParameters {
public:
	MaterialParameters() {}
	MaterialParameters(const std::string &aMaterialName, RenderStyle aRenderStyle, const MaterialParameterValues &aParameterValues)
		: mMaterialName(aMaterialName)
		, mRenderStyle(aRenderStyle)
		, mParameterValues(aParameterValues)
	{}

	std::string mMaterialName;
	RenderStyle mRenderStyle;
	MaterialParameterValues mParameterValues;
};

class AShaderProgram {

};

class ATexture {

};

class MaterialFactory {
public:
	virtual std::shared_ptr<AShaderProgram> getShaderProgram(const std::string &aName) = 0;
	virtual std::shared_ptr<ATexture> getTexture(const std::string &aName) = 0;
};

using ShaderFiles = std::map<std::string, fs::path>;
using ShaderProgramFiles = std::map<std::string, ShaderFiles>;

inline ShaderProgramFiles listShaderFiles(const fs::path& aShaderDir) {
	if (!fs::exists(aShaderDir) || !fs::is_directory(aShaderDir)) {
		throw std::runtime_error("Shader dir path is not a directory or does not exist.");
	}
	std::cout << "Loading shaders from directory: " << aShaderDir << "\n";

	std::map<std::string, std::regex> patterns = {
		{ "vertex", std::regex("(.*)\\.vertex\\.glsl") },
		{ "fragment", std::regex("(.*)\\.fragment\\.glsl") },
		{ "program", std::regex("(.*)\\.program") },
	};
	ShaderProgramFiles shaderFiles;

	for (const auto& entry : fs::directory_iterator(aShaderDir)) {
		if (!entry.is_regular_file()) {
			std::cout << "Skipping " << entry << "\n";
			continue;
		}
		std::string filename = entry.path().filename().string();
		std::smatch matches;

		for (const auto &pattern : patterns) {
			if (std::regex_match(filename, matches, pattern.second) && matches.size() == 2) {
				std::cout << "Adding " << filename << "\n";
				shaderFiles[pattern.first][matches[1]] = entry;
			}
		}
	}
	return shaderFiles;
}

inline std::map<std::string, std::string> parseProgramFile(const fs::path& filePath) {
	std::map<std::string, std::string> parsedData;
	std::ifstream file(filePath);

	if (!file.is_open()) {
		throw OpenGLError("Failed to open program file: " + filePath.string());
	}

	std::string line;
	while (std::getline(file, line)) {
		auto pos = line.find(':');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			// Trim whitespace using a lambda function and ranges
			auto trim = [](const std::string& str) {
				auto start = std::ranges::find_if_not(str, isspace);
				auto end = std::ranges::find_if_not(str | std::views::reverse, isspace);
				return start != str.end() ? std::string(start, end.base()) : "";
			};

			parsedData[trim(key)] = trim(value);
		}
	}

	return parsedData;
}

std::string loadShaderSource(const fs::path& filePath) {
	// Check if the file exists before trying to open it
	if (!fs::exists(filePath)) {
		throw OpenGLError("File does not exist: " + filePath.string());
	}

	std::ifstream fileStream(filePath, std::ios::binary); // Open in binary mode to preserve all data

	if (!fileStream.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath.string());
	}

	// Use iterators to read the file content into a string
	std::string fileContent((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());

	return fileContent;
}

class OGLShaderProgram: public AShaderProgram {
public:
	OGLShaderProgram(
		OpenGLResource &&aProgram,
		GLint modelMat,
		GLint viewMat,
		GLint projMat)
		: program(std::move(aProgram))
		, u_modelMat(modelMat)
		, u_viewMat(viewMat)
		, u_projMat(projMat)
	{}
	OpenGLResource program;
	GLint u_modelMat;
	GLint u_viewMat;
	GLint u_projMat;

	void use() const { GL_CHECK(glUseProgram(program.get())); }
	void setUniformMatrices(
			const glm::mat4 &aModel,
			const glm::mat4 &aView,
			const glm::mat4 &aProj) const
	{
		// std::cout << "Matrices: \n"
		// 	<< "Model " << glm::to_string(aModel) << "\n"
		// 	<< "View " << glm::to_string(aView) << "\n"
		// 	<< "Proj " << glm::to_string(aProj) << "\n";

		// GL_CHECK(glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(aModel)));
		GL_CHECK(glUniformMatrix4fv(u_modelMat, 1, GL_FALSE, glm::value_ptr(aModel)));
		GL_CHECK(glUniformMatrix4fv(u_viewMat, 1, GL_FALSE, glm::value_ptr(aView)));
		GL_CHECK(glUniformMatrix4fv(u_projMat, 1, GL_FALSE, glm::value_ptr(aProj)));
	}
};

class OGLTexture: public ATexture {
public:
	OpenGLResource texture;

};


class OGLMaterialFactory: public MaterialFactory {
public:
	void loadShadersFromDir(const fs::path& aShaderDir) {
		ShaderProgramFiles shaderFiles = listShaderFiles(aShaderDir);

		auto &vertexShaderFiles = shaderFiles["vertex"];
		std::map<std::string, OpenGLResource> vertexShadersCompiled;
		for (auto &shaderFile : vertexShaderFiles) {
			auto content = loadShaderSource(shaderFile.second);
			auto compiledShader = compileShader(GL_VERTEX_SHADER, content);
			vertexShadersCompiled.emplace(shaderFile.first, std::move(compiledShader));
		}
		auto &fragmentShaderFiles = shaderFiles["fragment"];
		std::map<std::string, OpenGLResource> fragmentShadersCompiled;
		for (auto &shaderFile : fragmentShaderFiles) {
			auto content = loadShaderSource(shaderFile.second);
			auto compiledShader = compileShader(GL_FRAGMENT_SHADER, content);
			fragmentShadersCompiled.emplace(shaderFile.first, std::move(compiledShader));
		}

		auto &programFiles = shaderFiles["program"];
		for (auto &programFile : programFiles) {
			auto shaders = parseProgramFile(programFile.second);

			auto vit = vertexShadersCompiled.find(shaders["vertex"]);
			auto fit = fragmentShadersCompiled.find(shaders["fragment"]);
			if (vit == vertexShadersCompiled.end()) {
				throw OpenGLError(
					"Program " + programFile.first + " cannot be linked. Vertex shader " +
				       	shaders["vertex"] + " was not compiled.");
			}
			if (fit == fragmentShadersCompiled.end()) {
				throw OpenGLError(
					"Program " + programFile.first + " cannot be linked. Fragment shader " +
				       	shaders["fragment"] + " was not compiled.");
			}
			auto program = createShaderProgram(vit->second, fit->second);
			GLint u_modelMat = glGetUniformLocation(program.get(), "u_modelMat");
			GLint u_viewMat = glGetUniformLocation(program.get(), "u_viewMat");
			GLint u_projMat = glGetUniformLocation(program.get(), "u_projMat");
			std::cout << "Model loc: " << u_modelMat << "\n";
			std::cout << "View loc: " << u_viewMat << "\n";
			std::cout << "Proj loc: " << u_projMat << "\n";
			mPrograms.emplace(
				programFile.first,
				std::make_shared<OGLShaderProgram>(
					std::move(program),
					u_modelMat,
					u_viewMat,
					u_projMat
					));
		}
	}

	void loadTexturesFromDir(const fs::path& aTextureDir) {
	}

	std::shared_ptr<AShaderProgram> getShaderProgram(const std::string &aName) {
		auto it = mPrograms.find(aName);
		if (it == mPrograms.end()) {
			throw OpenGLError("Shader program " + aName + " not found");
		}

		return it->second;
	};

	std::shared_ptr<ATexture> getTexture(const std::string &aName) {
		auto it = mTextures.find(aName);
		if (it == mTextures.end()) {
			throw OpenGLError("Texture " + aName + " not found");
		}

		return it->second;
	};

protected:
	using CompiledPrograms = std::map<std::string, std::shared_ptr<OGLShaderProgram>>;
	using Textures = std::map<std::string, std::shared_ptr<OGLTexture>>;

	CompiledPrograms mPrograms;
	Textures mTextures;
};

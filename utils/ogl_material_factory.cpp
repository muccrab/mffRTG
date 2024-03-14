#include "ogl_material_factory.hpp"
#include <iostream>
#include <ranges>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


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

void OGLMaterialFactory::loadShadersFromDir(fs::path aShaderDir) {
	aShaderDir = fs::canonical(aShaderDir);
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
		std::cout << "Creating shader program: " << programFile.first << "\n";
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
		auto uniforms = listShaderUniforms(program);
		for (auto info : uniforms) {
			std::cout
				<< "Uniform name: " << info.name
				<< " Type: " << getGLTypeName(info.type)
				<< " Location: " << info.location << "\n";
		}
		mPrograms.emplace(
				programFile.first,
				std::make_shared<OGLShaderProgram>(
					std::move(program),
					std::move(uniforms)
					));
	}
}

std::vector<fs::path> findImageFiles(const fs::path& aTextureDir) {
	if (!fs::exists(aTextureDir) || !fs::is_directory(aTextureDir)) {
		throw std::runtime_error("Texture dir path is not a directory or does not exist.");
	}
	std::cout << "Loading textures from directory: " << aTextureDir << "\n";
	std::vector<fs::path> imageFiles;

	for (const auto& entry : fs::recursive_directory_iterator(aTextureDir)) {
		if (entry.is_regular_file()) {
			auto ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(),
					[](unsigned char c){ return std::tolower(c); });

			if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tga") {
				imageFiles.push_back(entry.path());
			}
		}
	}

	return imageFiles;
}

struct ImageData {
	std::unique_ptr<unsigned char, void(*)(void*)> data;
	int width, height, channels;

	ImageData(unsigned char* data, int width, int height, int channels)
		: data(data, stbi_image_free), width(width), height(height), channels(channels) {}
};

std::unique_ptr<ImageData> loadImage(const fs::path& filePath) {
	int width, height, channels;
	unsigned char* data = stbi_load(filePath.string().c_str(), &width, &height, &channels, 0);
	if (!data) {
		throw OpenGLError("Failed to load image: " + filePath.string());
	}

	return std::make_unique<ImageData>(data, width, height, channels);
}

OpenGLResource createTextureFromData(const ImageData& imgData) {
	auto textureID = createTexture();
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID.get()));

	// Set texture wrapping and filtering options
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLenum format = GL_RGB;
	switch (imgData.channels) {
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		throw OpenGLError("Unsupported number of texture channels");
	};

	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, imgData.width, imgData.height, 0, format, GL_UNSIGNED_BYTE, imgData.data.get()));
	GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

	return textureID;
}


void OGLMaterialFactory::loadTexturesFromDir(fs::path aTextureDir) {
	aTextureDir = fs::canonical(aTextureDir);
	auto imageFiles = findImageFiles(aTextureDir);

	for (const auto &textureFile : imageFiles) {
		auto imageData = loadImage(textureFile);
		if (imageData) {
			auto texture = createTextureFromData(*imageData);

			auto name = fs::relative(textureFile, aTextureDir).string();
			mTextures[name] = std::make_shared<OGLTexture>(std::move(texture));
			std::cout << "Loaded texture: " << name << " from " << textureFile << "\n";
		}
	}
}

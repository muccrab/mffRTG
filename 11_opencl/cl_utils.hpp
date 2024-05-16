#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <filesystem>

// Function to save a buffer to a PNG file with a zero-padded index
inline void save_png(const std::filesystem::path& directory, const std::vector<cl_uchar>& buffer, int width, int height, int channels, int iteration) {
	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directories(directory);
	}
	// std::vector<cl_uchar> tmp(buffer.size());
	// std::transform(buffer.begin(), buffer.end(), tmp.begin(), [](const auto &val) { return val * 255; });
	std::ostringstream filename;
	filename << "output_" << std::setw(3) << std::setfill('0') << iteration << ".png";
	stbi_write_png((directory / filename.str()).c_str(), width, height, channels, buffer.data(), width);
}

// Function to load a grayscale image from a file
inline std::vector<cl_uchar> load_grayscale_image(const std::filesystem::path& filepath, int& width, int& height) {
	int channels;
	stbi_uc* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_grey);
	if (!data) {
		throw std::runtime_error("Failed to load image: " + filepath.string());
	}
	if (channels != 1) {
		stbi_image_free(data);
		std::cerr << channels << " AAAAAAAAAAA\n";
		throw std::runtime_error("Not a grayscale image: " + filepath.string());
	}
	std::vector<cl_uchar> buffer(data, data + width * height);
	stbi_image_free(data);
	return buffer;
}


// Function to load an OpenCL kernel source code from a text file
inline std::string load_kernel(const std::filesystem::path& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open kernel file: " + filepath.string());
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

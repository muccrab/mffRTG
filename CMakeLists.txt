cmake_minimum_required(VERSION 3.10)

# Set your project name
project(GLTutorials)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL package
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)


if(WIN32)
	# Attempt to find the GLFW root directory
	find_path(GLFW_ROOT_DIR NAMES include/GLFW/glfw3.h
		PATHS
			"${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4.bin.WIN64"
			"${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4"
			"${CMAKE_CURRENT_SOURCE_DIR}/../glfw-3.4.bin.WIN64"
			"${CMAKE_CURRENT_SOURCE_DIR}/../glfw-3.4"
		NO_DEFAULT_PATH) # Avoid searching in default paths

	if(GLFW_ROOT_DIR)
		# Determine the appropriate library directory based on the Visual Studio version
		if(MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1940)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2022")
		elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2019")
		elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2017")
			# Add more conditions for other Visual Studio versions as needed
		else()
			message(FATAL_ERROR "Unsupported Visual Studio version for GLFW.")
		endif()

		add_library(glfw INTERFACE)

		# Set the include directory for the glfw target
		target_include_directories(glfw INTERFACE "${GLFW_ROOT_DIR}/include")

		# Assuming the GLFW library filename; adjust as necessary
		find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${GLFW_LIB_DIR}" NO_DEFAULT_PATH)

		if(NOT GLFW_LIBRARY)
			message(FATAL_ERROR "GLFW library not found.")
		endif()

		# Link the library with the glfw target
		target_link_libraries(glfw INTERFACE "${GLFW_LIBRARY}")

		# Specify the include directory
		set(GLFW_INCLUDE_DIR "${GLFW_ROOT_DIR}/include")
	else()
		message(FATAL_ERROR "GLFW root directory not found.")
	endif()
else()
	find_package(glfw3 3.3 REQUIRED)
endif()


# Add subdirectories here
add_subdirectory(01_triangle)


# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(06_3d_textures)

add_executable(06_3d_textures 
	main.cpp
	../utils/error_handling.hpp
	../utils/ogl_resource.hpp
	../utils/shader.hpp
	../utils/window.hpp
)
target_sources(06_3d_textures PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(06_3d_textures utils glm::glm glfw OpenGL::GL)
target_include_directories(06_3d_textures PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)

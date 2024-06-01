# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(A01_particles)

add_executable(A01_particles 
	main.cpp
	../../utils/error_handling.hpp
	../../utils/ogl_resource.hpp
	../../utils/shader.hpp
	../../utils/window.hpp
)
target_sources(A01_particles PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../../glad/src/glad.c
)
target_link_libraries(A01_particles utils glm::glm glfw OpenGL::GL)
target_include_directories(A01_particles PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../utils
	${CMAKE_CURRENT_SOURCE_DIR}/../..
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(A02_SSAO)

add_executable(A02_SSAO 
	main.cpp
	renderer.hpp
	scene_definition.hpp
)
target_sources(A02_SSAO PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../../glad/src/glad.c
)
target_link_libraries(A02_SSAO utils glm::glm glfw OpenGL::GL)
target_include_directories(A02_SSAO PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../utils
	${CMAKE_CURRENT_SOURCE_DIR}/../..
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)

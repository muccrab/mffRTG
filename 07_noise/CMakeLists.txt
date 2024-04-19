# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(07_noise)

add_executable(07_noise 
	main.cpp
)
target_sources(07_noise PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(07_noise utils glm::glm glfw OpenGL::GL)
target_include_directories(07_noise PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)

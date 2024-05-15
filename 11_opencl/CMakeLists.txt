
# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(11_opencl)


list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/clFFT-Full-2.12.2-Linux-x64/lib64/cmake")

find_package(clFFT REQUIRED)


add_executable(11_opencl 
	main.cpp
)
# target_sources(11_opencl PRIVATE 
# 	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
# )
# target_link_libraries(11_opencl utils glm::glm glfw OpenGL::GL)
target_link_libraries(11_opencl clFFT)
target_include_directories(11_opencl PRIVATE
	# ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	# ${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/clFFT-Full-2.12.2-Linux-x64/include
	/usr/local/cuda-12.1/targets/x86_64-linux/include
)

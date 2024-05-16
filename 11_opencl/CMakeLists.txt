
# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(11_opencl)

find_package(OpenCL REQUIRED)

# list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/clFFT-install/lib64/cmake")
#
# find_package(clFFT REQUIRED)


# add_executable(11_opencl 
# 	main.cpp
# )
# # target_sources(11_opencl PRIVATE 
# # 	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
# # )
# # target_link_libraries(11_opencl utils glm::glm glfw OpenGL::GL)
# target_link_libraries(11_opencl OpenCL::OpenCL clFFT )
# target_include_directories(11_opencl PRIVATE
# 	# ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
# 	# ${CMAKE_CURRENT_SOURCE_DIR}/../utils
# 	${CMAKE_CURRENT_SOURCE_DIR}/..
# 	${CMAKE_CURRENT_SOURCE_DIR}
# 	${CMAKE_CURRENT_SOURCE_DIR}/clFFT-Full-2.12.2-Linux-x64/include
# 	# /usr/local/cuda-12.1/targets/x86_64-linux/include
# )

add_executable(conway
	conway.cpp
)
# target_sources(11_opencl PRIVATE 
# 	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
# )
# target_link_libraries(11_opencl utils glm::glm glfw OpenGL::GL)
target_link_libraries(conway OpenCL::OpenCL)
target_include_directories(conway PRIVATE
	# ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	# ${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
	# /usr/local/cuda-12.1/targets/x86_64-linux/include
)

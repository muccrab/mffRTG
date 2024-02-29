cmake_minimum_required(VERSION 3.10)

# Set your project name
project(GLTutorials)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL package
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# If you are using GLFW and GLAD, make sure they are available in the project,
# e.g., by including them as submodules or finding them with find_package
# For GLAD, you might need to add its source files to your executable manually
# if it's not available as a findable package.

# For GLFW, you can find it via find_package if it's installed
find_package(glfw3 3.3 REQUIRED)

# Add subdirectories here
add_subdirectory(01_triangle)


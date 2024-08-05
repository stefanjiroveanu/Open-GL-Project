cmake_minimum_required(VERSION 3.24)
project(Lab9)

set(CMAKE_CXX_STANDARD 14)

include_directories(../Lab9)

add_executable(Lab9 ../Lab9/main.cpp ../Lab9/Camera.cpp ../Lab9/Camera.hpp ../Lab9/Mesh.hpp ../Lab9/Mesh.cpp ../Lab9/Model3D.hpp ../Lab9/Model3D.cpp ../Lab9/Shader.cpp ../Lab9/Shader.hpp ../Lab9/stb_image.h ../Lab9/stb_image.cpp ../Lab9/tiny_obj_loader.h ../Lab9/tiny_obj_loader.cpp ../Lab9/SkyBox.cpp ../Lab9/SkyBox.hpp)
target_link_libraries(Lab9 glfw GLEW GL )
cmake_minimum_required(VERSION 3.27)
project(Global)

set(CMAKE_CXX_STANDARD 20)
set(GL_Dir E:/LEARN/Self-learn\ mod/Computer\ GL/Needed)

add_executable(Global ${GL_Dir}/src/glad.c main.cpp)

INCLUDE_DIRECTORIES(${GL_Dir}/include)
INCLUDE_DIRECTORIES(${GL_Dir}/stb-master)

link_directories(${GL_Dir}/lib)
target_link_libraries(Global ${GL_Dir}/lib/glfw3.dll)
cmake_minimum_required(VERSION 3.30)
project(MyFirstTriangle)

set(CMAKE_CXX_STANDARD 17)

include_directories("externalLibs/include")

link_directories("externalLibs/lib")

link_directories("externalLibs/bin")


# Add your executable target
add_executable(MyFirstTriangle src/main.cpp)

target_link_libraries(MyFirstTriangle glew32 glfw3 opengl32)








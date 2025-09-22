cmake_minimum_required(VERSION 3.30)
project(deneme)

set(CMAKE_CXX_STANDARD 17)

include_directories("./include")

link_directories("./lib")



add_executable(deneme main.cpp
        main.cpp)

target_link_libraries(deneme glew32 glfw3 opengl32)